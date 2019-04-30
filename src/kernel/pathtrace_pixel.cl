/* Structures that are shared between host/device */

typedef struct __attribute__ ((packed)) bvh_node {
  float3 min;
  float3 max;
  uint prim_offset;
  uint prim_count;
  uint left_offset;
  uint right_offset;
} bvh_node_t;

#define PRIMITIVE_TYPE_SPHERE 0
typedef struct __attribute__ ((packed)) sphere {
  uchar type;
  float3 origin;
  float radius;
  uchar padding[60];
} sphere_t;

#define PRIMITIVE_TYPE_TRIANGLE 1
typedef struct __attribute__ ((packed)) triangle {
  uchar type;
  float3 vertices[3];
  float3 normals[3];
} triangle_t;

typedef union __attribute__ ((packed)) primitive {
  uchar type;
  sphere_t sphere;
  triangle_t triangle;
} primitive_t;

typedef struct __attribute__ ((packed)) mat3 {
  float3 c0, c1, c2;
} mat3_t;

typedef struct __attribute__ ((packed)) camera {
  float h_fov, v_fov, n_clip, f_clip;
  float3 pos;
  mat3_t c2w;
} camera_t;

/* Structures that are private to the device */

typedef struct ray {
  float3 o;
  float3 d;
  float min_t;
  float max_t;
} ray_t;

typedef struct intersection {
  float t;
  global primitive_t *primitive;
  float3 n;
} intersection_t;

/* Utility functions */

float3 mat_mul(mat3_t *mat, float3 *vec) {
  return vec->x * mat->c0 + vec->y * mat->c1 + vec->z * mat->c2;
}

float rand(uint* seed) {
  int const a = 16807; //ie 7**5
  int const m = 2147483647; //ie 2**31-1

  *seed = ((long)(*seed * a)) % m;
  return (float) (*seed) / m;
}

/* Path tracing functions */

void generate_ray(camera_t *camera, float cx, float cy, ray_t *output) {
  float3 dir = (float3)((2 * cx - 1) * tan(camera->h_fov * 0.5f),
                        (2 * cy - 1) * tan(camera->v_fov * 0.5f),
                        -1);
  dir = mat_mul(&camera->c2w, &dir);
  output->o = camera->pos;
  output->d = normalize(dir);
  output->min_t = camera->n_clip;
  output->max_t = camera->f_clip;
}

bool intersect_triangle(ray_t *ray, global triangle_t *triangle, intersection_t *isect) {
  float3 v0v1 = triangle->vertices[1] - triangle->vertices[0];
  float3 v0v2 = triangle->vertices[2] - triangle->vertices[0];
  float3 pvec = cross(ray->d, v0v2);
  float det = dot(v0v1, pvec);
  if (fabs(det) <= 0.0f) {
    return false;
  }
  float invDet = 1.0f / det;
  float3 tvec = ray->o - triangle->vertices[0];
  float u = dot(tvec, pvec) * invDet;
  if (u < 0.0f || u > 1.0f) {
    return false;
  }

  float3 qvec = cross(tvec, v0v1);
  float v = dot(ray->d, qvec) * invDet;
  if (v < 0.0f || u + v > 1.0f) {
    return false;
  }

  float t = dot(v0v2, qvec) * invDet;
  if (t < ray->min_t || t > ray->max_t) {
    return false;
  }

  if (isect) {
    isect->t = t;
    isect->primitive = (global primitive_t *) triangle;
    isect->n = normalize((1.0f - u - v) * triangle->normals[0]
    isect->n = (1.f - u - v) * triangle->normals[0]
               + u * triangle->normals[1]
               + v * triangle->normals[2]);
  }

  ray->max_t = t;
  return true;
}

bool intersect(ray_t *ray, global primitive_t *primitive, intersection_t *isect) {
  if (primitive->type == PRIMITIVE_TYPE_TRIANGLE) {
    return intersect_triangle(ray, &primitive->triangle, isect);
  } else if (primitive->type == PRIMITIVE_TYPE_SPHERE) {
    return false;
  } else {
    return false;
  }
}

float3 est_radiance_global_illumination(ray_t *ray,
                                        global primitive_t *primitives,
                                        uint num_primitives) {
  intersection_t isect;
  bool intersected = false;
  for (uint i = 0; i < num_primitives; i++) {
    intersected = intersect(ray, &primitives[i], &isect) || intersected;
  }
  if (intersected) {
    return (isect.n * 0.5f) + (float3)(0.5f, 0.5f, 0.5f);
  }
  return (float3)(0, 0, 0);
}

kernel void
pathtrace_pixel(global float4 *output_image,
                uint2 dimensions,
                uint num_samples,
                camera_t camera,
                global primitive_t *primitives,
                uint num_primitives)
{
  uint seed = get_global_id(1) * get_global_size(0) + get_global_id(0);
  uint x = get_global_id(0);
  uint y = get_global_id(1);
  if (x >= dimensions.x || y >= dimensions.y) {
    // We might have extra work units here since we need to evenly divide total
    // units with local units.
    return;
  }

  float3 total = (float3)(0, 0, 0);
  if (num_samples == 1) {
    ray_t ray;
    generate_ray(&camera,
                  (x + 0.5f) / dimensions.x,
                  (y + 0.5f) / dimensions.y,
                  &ray);
    total = est_radiance_global_illumination(&ray, primitives, num_primitives);
  } else {
    for (int i = 0; i < num_samples; i++) {
      ray_t ray;
      generate_ray(&camera,
                   (x + rand(&seed)) / dimensions.x,
                   (y + rand(&seed)) / dimensions.y,
                   &ray);
      total += est_radiance_global_illumination(&ray, primitives, num_primitives);
    }
  }
  output_image[y * dimensions.x + x] = (float4)(total / num_samples, 1);
}

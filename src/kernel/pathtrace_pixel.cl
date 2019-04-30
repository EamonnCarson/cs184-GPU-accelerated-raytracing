/* Structures that are shared between host/device */

typedef struct __attribute__ ((packed)) bvh_node {
  float3 bounds[2];
  uint prim_index;
  uint prim_count;
  uint entry_index; // Index of node to jump to on intersection success
  uint exit_index; // Index of node to jump to on intersection failure
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

/* Intersection functions */

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
    isect->n = normalize((1.f - u - v) * triangle->normals[0]
                         + u * triangle->normals[1]
                         + v * triangle->normals[2]);
  }

  ray->max_t = t;
  return true;
}

bool intersect_sphere(ray_t *ray, global sphere_t *sphere, intersection_t *isect) {
  float a = dot(ray->d, ray->d);
  float b = dot(2 * (ray->o - sphere->origin), ray->d);
  float c = dot(ray->o - sphere->origin, ray->o - sphere->origin)
            - sphere->radius * sphere->radius;
  float det = b * b - 4 * a * c;
  if (det < 0) {
    return false;
  }

  float sqrt_det = sqrt(det);
  float t = (-b - sqrt_det) / (2 * a);
  if (t < 0) {
    t = (-b + sqrt_det) / (2 * a);
  }

  if (t < ray->min_t || t > ray->max_t) {
    return false;
  }

  if (isect) {
    isect->t = t;
    isect->primitive = (global primitive_t *) sphere;
    isect->n = normalize((ray->o + ray->d * t) - sphere->origin);
  }

  ray->max_t = t;

  return true;
}

bool intersect_primitive(ray_t *ray,
                         global primitive_t *primitive,
                         intersection_t *isect) {
  if (primitive->type == PRIMITIVE_TYPE_TRIANGLE) {
    return intersect_triangle(ray, &primitive->triangle, isect);
  } else if (primitive->type == PRIMITIVE_TYPE_SPHERE) {
    return intersect_sphere(ray, &primitive->sphere, isect);
  } else {
    return false;
  }
}

bool intersect_bvh_bbox(ray_t *ray,
                        global bvh_node_t *bvh_node,
                        float *t0,
                        float *t1) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    bool xsign = ray->d.x < 0;
    bool ysign = ray->d.y < 0;
    bool zsign = ray->d.z < 0;

    tmin = (bvh_node->bounds[xsign].x - ray->o.x) / ray->d.x;
    tmax = (bvh_node->bounds[1-xsign].x - ray->o.x) / ray->d.x;
    tymin = (bvh_node->bounds[ysign].y - ray->o.y) / ray->d.y;
    tymax = (bvh_node->bounds[1-ysign].y - ray->o.y) / ray->d.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bvh_node->bounds[zsign].z - ray->o.z) / ray->d.z;
    tzmax = (bvh_node->bounds[1-zsign].z - ray->o.z) / ray->d.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    *t0 = tmin;
    *t1 = tmax;
    return true;
}

/** Intersection test for a flattened BVH */
bool intersect_bvh(ray_t *ray,
                   global bvh_node_t *bvh,
                   global primitive_t *primitives,
                   intersection_t *isect) {
  float t0, t1;
  bool intersects = false;
  uint next_node_index = 0;
  do {
    global bvh_node_t *curr_node = &bvh[next_node_index];

    if (!intersect_bvh_bbox(ray, curr_node, &t0, &t1)
        || t0 > ray->max_t
        || t1 < ray->min_t) {
      next_node_index = curr_node->exit_index;
    } else {
      if (curr_node->prim_count > 0) { // Leaf node
        for (uint i = curr_node->prim_index;
             i < curr_node->prim_index + curr_node->prim_count;
             i++) {
          intersects = intersect_primitive(ray, &primitives[i], isect)
                       || intersects;
        }
        // For leaf nodes, entry_index == exit_index
      }
      next_node_index = curr_node->entry_index;
    }
  } while (next_node_index != 0);
  return intersects;
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

float3 est_radiance_global_illumination(ray_t *ray,
                                        global bvh_node_t *bvh,
                                        global primitive_t *primitives) {
  intersection_t isect;
  bool intersected = intersect_bvh(ray, bvh, primitives, &isect);
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
                global bvh_node_t *bvh,
                global primitive_t *primitives)
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
  for (int i = 0; i < num_samples; i++) {
    ray_t ray;
    if (num_samples == 1) {
      generate_ray(&camera,
                   (x + 0.5f) / dimensions.x,
                   (y + 0.5f) / dimensions.y,
                   &ray);
    } else {
      generate_ray(&camera,
                   (x + rand(&seed)) / dimensions.x,
                   (y + rand(&seed)) / dimensions.y,
                   &ray);
    }
    total += est_radiance_global_illumination(&ray, bvh, primitives);
  }
  output_image[y * dimensions.x + x] = (float4)(total / num_samples, 1);
}

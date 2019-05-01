#ifndef KERNEL_INTERSECT_H
#define KERNEL_INTERSECT_H

#include "types.h"

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

#endif // KERNEL_INTERSECT_H

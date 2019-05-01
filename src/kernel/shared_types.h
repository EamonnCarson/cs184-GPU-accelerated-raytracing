#ifndef KERNEL_SHARED_TYPES_H
#define KERNEL_SHARED_TYPES_H

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

#endif // KERNEL_SHARED_TYPES_H

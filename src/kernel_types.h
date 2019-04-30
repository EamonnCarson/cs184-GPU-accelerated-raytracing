#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <CL/cl.hpp>

#include "CGL/vector3D.h"
#include "CGL/matrix3x3.h"

/* Structs that are passed from host -> device */

#pragma pack(push, 1)

typedef struct kernel_mat3 {
  cl_float3 r0, r1, r2;
} kernel_mat3_t;

typedef struct kernel_camera {
  cl_float h_fov, v_fov, n_clip, f_clip;
  cl_float3 pos;
  kernel_mat3_t c2w;
} kernel_camera_t;

#define KERNEL_PRIMITIVE_TYPE_SPHERE 0
typedef struct kernel_sphere {
  cl_uchar type;
  cl_float3 origin;
  cl_float radius;
  cl_uchar padding[60];
} kernel_sphere_t;

#define KERNEL_PRIMITIVE_TYPE_TRIANGLE 1
typedef struct kernel_triangle {
  cl_uchar type;
  cl_float3 vertices[3];
  cl_float3 normals[3];
} kernel_triangle_t;

typedef union kernel_primitive {
  cl_uchar type;
  kernel_sphere_t sphere;
  kernel_triangle_t triangle;
} kernel_primitive_t;

typedef struct kernel_bvh_node {
  cl_float3 bounds[2];
  cl_uint prim_index;
  cl_uint prim_count;
  cl_uint entry_index; // Index of node to jump to on intersection success
  cl_uint exit_index; // Index of node to jump to on intersection failure
} kernel_bvh_node_t;

#pragma pack(pop)

cl_float3 cglVectorToKernel(CGL::Vector3D vector, bool normalize = false);

kernel_mat3_t cglMatrixToKernel(CGL::Matrix3x3 matrix);

#endif // KERNEL_TYPES_H

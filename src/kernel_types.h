#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <CL/cl.hpp>

#include "CGL/vector3D.h"
#include "CGL/matrix3x3.h"

/* Structs that are passed from host -> device */

typedef struct __attribute__ ((packed)) kernel_mat3 {
  cl_float3 r0, r1, r2;
} kernel_mat3_t;

typedef struct __attribute__ ((packed)) kernel_camera {
  cl_float h_fov, v_fov, n_clip, f_clip;
  cl_float3 pos;
  kernel_mat3_t c2w;
} kernel_camera_t;

#define KERNEL_PRIMITIVE_TYPE_SPHERE 0
typedef struct __attribute__ ((packed)) kernel_sphere {
  cl_uchar type;
  cl_float3 origin;
  cl_float radius;
  cl_uchar padding[60];
} kernel_sphere_t;

#define KERNEL_PRIMITIVE_TYPE_TRIANGLE 1
typedef struct __attribute__ ((packed)) kernel_triangle {
  cl_uchar type;
  cl_float3 vertices[3];
  cl_float3 normals[3];
} kernel_triangle_t;

typedef union __attribute__ ((packed)) kernel_primitive {
  cl_uchar type;
  kernel_sphere_t sphere;
  kernel_triangle_t triangle;
} kernel_primitive_t;

cl_float3 cglVectorToKernel(CGL::Vector3D vector);

kernel_mat3 cglMatrixToKernel(CGL::Matrix3x3 matrix);

#endif // KERNEL_TYPES_H

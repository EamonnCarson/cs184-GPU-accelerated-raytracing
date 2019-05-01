#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <CL/cl.hpp>

#include "CGL/vector3D.h"
#include "CGL/spectrum.h"
#include "CGL/matrix3x3.h"

/* Structs that are passed from host -> device */

#pragma pack(push, 1)

/* BSDF */

#define KERNEL_BSDF_TYPE_DIFFUSE 0
typedef struct kernel_diffuse_bsdf {
  cl_float3 reflectance;
  cl_float padding[4];
} kernel_diffuse_bsdf_t;

#define KERNEL_BSDF_TYPE_MIRROR 1
typedef struct kernel_mirror_bsdf {
  cl_float3 reflectance;
  cl_float padding[4];
} kernel_mirror_bsdf_t;

#define KERNEL_BSDF_TYPE_MICROFACET 2
typedef struct kernel_microfacet_bsdf {
  cl_float3 eta;
  cl_float3 k;
  cl_float alpha;
} kernel_microfacet_bsdf_t;

#define KERNEL_BSDF_TYPE_GLASS 3
typedef struct kernel_glass_bsdf {
  cl_float ior;
  cl_float3 reflectance;
  cl_float3 transmittance;
} kernel_glass_bsdf_t;

#define KERNEL_BSDF_TYPE_EMISSION 4
typedef struct kernel_emission_bsdf {
  cl_float3 radiance;
  cl_float padding[4];
} kernel_emission_bsdf_t;

typedef union kernel_bsdf_union {
  kernel_diffuse_bsdf_t diffuse;
  kernel_mirror_bsdf_t mirror;
  kernel_microfacet_bsdf_t microfacet;
  kernel_glass_bsdf_t glass;
  kernel_emission_bsdf_t emission;
} kernel_bsdf_union_t;

typedef struct kernel_bsdf {
  cl_uchar type;
  kernel_bsdf_union_t u;
} kernel_bsdf_t;

/* Lights */

#define KERNEL_LIGHT_TYPE_DIRECTIONAL 0
typedef struct kernel_directional_light {
  cl_float3 radiance;
  cl_float3 dir_to_light;
  cl_float3 padding[3];
} kernel_directional_light_t;

#define KERNEL_LIGHT_TYPE_HEMISPHERE 1
typedef struct kernel_hemisphere_light {
  cl_float3 radiance;
  cl_float3 padding[4];
} kernel_hemisphere_light_t;

#define KERNEL_LIGHT_TYPE_POINT 2
typedef struct kernel_point_light {
  cl_float3 radiance;
  cl_float3 position;
  cl_float3 padding[3];
} kernel_point_light_t;

#define KERNEL_LIGHT_TYPE_AREA 3
typedef struct kernel_area_light {
  cl_float3 radiance;
  cl_float3 position;
  cl_float3 direction;
  cl_float3 dim_x;
  cl_float3 dim_y;
} kernel_area_light_t;

typedef union kernel_light_union {
  kernel_directional_light_t directional;
  kernel_hemisphere_light_t hemisphere;
  kernel_point_light_t point;
  kernel_area_light_t area;
} kernel_light_union_t;

typedef struct kernel_light {
  cl_uchar type;
  kernel_light_union_t u;
} kernel_light_t;

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
  cl_uint bsdf_index;
  cl_uchar padding[60];
} kernel_sphere_t;

#define KERNEL_PRIMITIVE_TYPE_TRIANGLE 1
typedef struct kernel_triangle {
  cl_uchar type;
  cl_float3 vertices[3];
  cl_float3 normals[3];
  cl_uint bsdf_index;
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

cl_float3 cglSpectrumToKernel(CGL::Spectrum spectrum);

kernel_mat3_t cglMatrixToKernel(CGL::Matrix3x3 matrix);

#endif // KERNEL_TYPES_H

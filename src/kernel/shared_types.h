#ifndef KERNEL_SHARED_TYPES_H
#define KERNEL_SHARED_TYPES_H

/* Structures that are shared between host/device */

#define BSDF_TYPE_DIFFUSE 0
typedef struct __attribute__ ((packed)) diffuse_bsdf {
  float3 reflectance;
  float padding[4];
} diffuse_bsdf_t;

#define BSDF_TYPE_MIRROR 1
typedef struct __attribute__ ((packed)) mirror_bsdf {
  float3 reflectance;
  float padding[4];
} mirror_bsdf_t;

#define BSDF_TYPE_MICROFACET 2
typedef struct __attribute__ ((packed)) microfacet_bsdf {
  float3 eta;
  float3 k;
  float alpha;
} microfacet_bsdf_t;

#define BSDF_TYPE_GLASS 3
typedef struct __attribute__ ((packed)) glass_bsdf {
  float ior;
  float3 reflectance;
  float3 transmittance;
} glass_bsdf_t;

#define BSDF_TYPE_EMISSION 4
typedef struct __attribute__ ((packed)) emission_bsdf {
  float3 radiance;
  float padding[4];
} emission_bsdf_t;

typedef union __attribute__ ((packed)) bsdf_union {
  diffuse_bsdf_t diffuse;
  mirror_bsdf_t mirror;
  microfacet_bsdf_t microfacet;
  glass_bsdf_t glass;
  emission_bsdf_t emission;
} bsdf_union_t;

typedef struct __attribute__ ((packed)) bsdf {
  uchar type;
  bsdf_union_t u;
} bsdf_t;

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
  uint bsdf_index;
  uchar padding[60];
} sphere_t;

#define PRIMITIVE_TYPE_TRIANGLE 1
typedef struct __attribute__ ((packed)) triangle {
  uchar type;
  float3 vertices[3];
  float3 normals[3];
  uint bsdf_index;
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

/* Lights */

#define LIGHT_TYPE_DIRECTIONAL 0
typedef struct __attribute__ ((packed)) directional_light {
  float3 radiance;
  float3 dir_to_light;
  float3 padding[3];
} directional_light_t;

#define LIGHT_TYPE_HEMISPHERE 1
typedef struct __attribute__ ((packed)) hemisphere_light {
  float3 radiance;
  float3 padding[4];
} hemisphere_light_t;

#define LIGHT_TYPE_POINT 2
typedef struct __attribute__ ((packed)) point_light {
  float3 radiance;
  float3 position;
  float3 padding[3];
} point_light_t;

#define LIGHT_TYPE_AREA 3
typedef struct __attribute__ ((packed)) area_light {
  float3 radiance;
  float3 position;
  float3 direction;
  float3 dim_x;
  float3 dim_y;
} area_light_t;

typedef union __attribute__ ((packed)) light_union {
  directional_light_t directional;
  hemisphere_light_t hemisphere;
  point_light_t point;
  area_light_t area;
} light_union_t;

typedef struct __attribute__ ((packed)) light {
  uchar type;
  light_union_t u;
} light_t;

#endif // KERNEL_SHARED_TYPES_H

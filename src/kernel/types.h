#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include "shared_types.h"

/* Structures that are private to the device */

typedef struct ray {
  float3 o;
  float3 d;
  float min_t;
  float max_t;
} ray_t;

typedef struct intersection {
  float t;
  uint bsdf_index;
  float3 n;
} intersection_t;

typedef uint rand_state_t;

/** Convenience struct for passing around common constants/state */
typedef struct global_state {
  rand_state_t *rand_state;
  uint light_samples;
  uint max_ray_depth;
  global bvh_node_t *bvh;
  global primitive_t *primitives;
  global light_t *lights;
  uint light_count;
  global bsdf_t *bsdfs;
} global_state_t;

#endif // KERNEL_TYPES_H

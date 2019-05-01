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
  global primitive_t *primitive;
  float3 n;
} intersection_t;

typedef uint rand_state_t;

#endif // KERNEL_TYPES_H

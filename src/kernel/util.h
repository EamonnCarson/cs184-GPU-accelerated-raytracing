#ifndef KERNEL_UTIL_H
#define KERNEL_UTIL_H

#include "types.h"

float3 mat_mul(mat3_t *mat, float3 *vec) {
  return vec->x * mat->c0 + vec->y * mat->c1 + vec->z * mat->c2;
}

float rand(rand_state_t* seed) {
  int const a = 16807; //ie 7**5
  int const m = 2147483647; //ie 2**31-1

  *seed = ((long)(*seed * a)) % m;
  return (float) (*seed) / m;
}

#endif // KERNEL_UTIL_H

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

bool coin_flip(float p, global_state_t *globals) {
  return rand(globals->rand_state) < p;
}

void reflect(float3 *wo, float3 *wi) {
  *wi = (float3)(-wo->x, -wo->y, wo->z);
}

bool refract(float3 *wo, float3 *wi, float ior) {
  float eta;
  int z_mult;
  if (wo->z < 0) {
    z_mult = 1;
    eta = ior;
  } else {
    z_mult = -1;
    eta = 1.f / ior;
  }

  float z_sq = 1 - (eta * eta) * (1 - (wo->z * wo->z));
  if (z_sq < 0) {
    return false;
  }
  *wi = (float3)(-eta * wo->x, -eta * wo->y, z_mult * sqrt(z_sq));
  return true;
}

#endif // KERNEL_UTIL_H

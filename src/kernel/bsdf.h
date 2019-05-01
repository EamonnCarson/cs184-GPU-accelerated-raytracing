#ifndef KERNEL_BSDF_H
#define KERNEL_BSDF_H

#include "types.h"

void bsdf_sample_f(global bsdf_t *bsdf,
                   float3 *wo,
                   float3 *reflectance,
                   float3 *wi,
                   float *pdf,
                   global_state_t *globals) {
  // TODO(willsheu)
}

#endif // KERNEL_BSDF_H

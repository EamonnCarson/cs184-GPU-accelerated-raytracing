#ifndef KERNEL_LIGHT_H
#define KERNEL_LIGHT_H

#include "types.h"

void light_sample_l(global light_t *light,
                    float3 *p,
                    float3 *radiance,
                    float3 *wi,
                    float *dist_to_light,
                    float *pdf,
                    global_state_t *globals) {
  // TODO(willsheu)
}

#endif // KERNEL_LIGHT_H

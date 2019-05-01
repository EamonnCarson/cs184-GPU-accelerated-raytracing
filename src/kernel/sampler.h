#ifndef KERNEL_SAMPLER_H
#define KERNEL_SAMPLER_H

#include "types.h"
#include "util.h"

void sample_uniform_hemisphere(float3 *sample,
                               global_state_t *globals) {
  float xi1 = rand(globals->rand_state);
  float xi2 = rand(globals->rand_state);

  float theta = acos(xi1);
  float phi = 2.f * M_PI_F * xi2;

  *sample = normalize((float3)(sin(theta) * cos(phi),
                               sin(theta) * sin(phi),
                               cos(theta)));
}

void sample_cosine_hemisphere(float3 *sample,
                              float *pdf,
                              global_state_t *globals) {
  float xi1 = rand(globals->rand_state);
  float xi2 = rand(globals->rand_state);

  float r = sqrt(xi1);
  float theta = 2.f * M_PI_F * xi2;
  *pdf = sqrt(1.f - xi1) * M_1_PI_F;
  *sample = normalize((float3)(r * cos(theta),
                               r * sin(theta),
                               sqrt(1.f - xi1)));
}

void sample_uniform_grid(float2 *sample,
                         global_state_t *globals) {
  sample->s0 = rand(globals->rand_state);
  sample->s1 = rand(globals->rand_state);
}

#endif // KERNEL_SAMPLER_H

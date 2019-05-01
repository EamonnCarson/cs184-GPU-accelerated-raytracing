#ifndef KERNEL_BSDF_H
#define KERNEL_BSDF_H

#include "types.h"
#include "util.h"
#include "sampler.h"

void diffuse_bsdf_f(global diffuse_bsdf_t *bsdf,
                    float3 *wo,
                    float3 *wi,
                    float3 *reflectance,
                    global_state_t *globals) {
  *reflectance = bsdf->reflectance / M_PI_F;
}

void diffuse_bsdf_sample_f(global diffuse_bsdf_t *bsdf,
                           float3 *wo,
                           float3 *reflectance,
                           float3 *wi,
                           float *pdf,
                           global_state_t *globals) {
  sample_cosine_hemisphere(wi, pdf, globals);
  *reflectance = bsdf->reflectance / M_PI_F;
}

void mirror_bsdf_f(global mirror_bsdf_t *bsdf,
                   float3 *wo,
                   float3 *wi,
                   float3 *reflectance,
                   global_state_t *globals) {
  *reflectance = (float3)(0, 0, 0);
}

void mirror_bsdf_sample_f(global mirror_bsdf_t *bsdf,
                          float3 *wo,
                          float3 *reflectance,
                          float3 *wi,
                          float *pdf,
                          global_state_t *globals) {
  *pdf = 1.f;
  reflect(wo, wi);
  *reflectance = bsdf->reflectance / fabs(wi->z);
}

void microfacet_bsdf_f(global microfacet_bsdf_t *bsdf,
                       float3 *wo,
                       float3 *wi,
                       float3 *reflectance,
                       global_state_t *globals) {
  // TODO: Implement
  *reflectance = (float3)(0, 0, 0);
}

void microfacet_bsdf_sample_f(global microfacet_bsdf_t *bsdf,
                              float3 *wo,
                              float3 *reflectance,
                              float3 *wi,
                              float *pdf,
                              global_state_t *globals) {
  // TODO: Implement
  *reflectance = (float3)(0, 0, 0);
}

void glass_bsdf_f(global glass_bsdf_t *bsdf,
                  float3 *wo,
                  float3 *wi,
                  float3 *reflectance,
                  global_state_t *globals) {
  *reflectance = (float3)(0, 0, 0);
}

void glass_bsdf_sample_f(global glass_bsdf_t *bsdf,
                         float3 *wo,
                         float3 *reflectance,
                         float3 *wi,
                         float *pdf,
                         global_state_t *globals) {
  if (!refract(wo, wi, bsdf->ior)) {
    reflect(wo, wi);
    *pdf = 1.f;
    *reflectance = bsdf->reflectance / fabs(wi->z);
  } else {
    float r0 = pow((1 - bsdf->ior) / (1 + bsdf->ior), 2);
    float r = r0 + (1.f - r0) * pow(1 - fabs(wo->z), 5);
    if (coin_flip(r, globals)) {
      reflect(wo, wi);
      *pdf = 1.f;
      *reflectance = r * bsdf->reflectance / fabs(wi->z);
    } else {
      *pdf = 1.f - r;
      float eta = wo->z < 0 ? bsdf->ior : 1.f / bsdf->ior;
      *reflectance = (1.f - r) * bsdf->transmittance / fabs(wi->z) / (eta * eta);
    }
  }
}

void emission_bsdf_f(global emission_bsdf_t *bsdf,
                     float3 *wo,
                     float3 *wi,
                     float3 *reflectance,
                     global_state_t *globals) {
  *reflectance = (float3)(0, 0, 0);
}

void emission_bsdf_sample_f(global emission_bsdf_t *bsdf,
                            float3 *wo,
                            float3 *reflectance,
                            float3 *wi,
                            float *pdf,
                            global_state_t *globals) {
  *pdf = 1.f * M_1_PI_F;
  sample_cosine_hemisphere(wi, pdf, globals);
  *reflectance = (float3)(0, 0, 0);
}

void bsdf_f(global bsdf_t *bsdf,
            float3 *wo,
            float3 *wi,
            float3 *reflectance,
            global_state_t *globals) {
  switch (bsdf->type) {
    case BSDF_TYPE_DIFFUSE:
      diffuse_bsdf_f(&bsdf->u.diffuse, wo, wi, reflectance, globals);
      break;
    case BSDF_TYPE_MIRROR:
      mirror_bsdf_f(&bsdf->u.mirror, wo, wi, reflectance, globals);
      break;
    case BSDF_TYPE_MICROFACET:
      microfacet_bsdf_f(&bsdf->u.microfacet, wo, wi, reflectance, globals);
      break;
    case BSDF_TYPE_GLASS:
      glass_bsdf_f(&bsdf->u.glass, wo, wi, reflectance, globals);
      break;
    case BSDF_TYPE_EMISSION:
      emission_bsdf_f(&bsdf->u.emission, wo, wi, reflectance, globals);
      break;
    default:
      // ERROR!
      return;
  }
}

void bsdf_sample_f(global bsdf_t *bsdf,
                   float3 *wo,
                   float3 *reflectance,
                   float3 *wi,
                   float *pdf,
                   global_state_t *globals) {
  switch (bsdf->type) {
    case BSDF_TYPE_DIFFUSE:
      diffuse_bsdf_sample_f(&bsdf->u.diffuse, wo, reflectance, wi, pdf, globals);
      break;
    case BSDF_TYPE_MIRROR:
      mirror_bsdf_sample_f(&bsdf->u.mirror, wo, reflectance, wi, pdf, globals);
      break;
    case BSDF_TYPE_MICROFACET:
      microfacet_bsdf_sample_f(&bsdf->u.microfacet, wo, reflectance, wi, pdf, globals);
      break;
    case BSDF_TYPE_GLASS:
      glass_bsdf_sample_f(&bsdf->u.glass, wo, reflectance, wi, pdf, globals);
      break;
    case BSDF_TYPE_EMISSION:
      emission_bsdf_sample_f(&bsdf->u.emission, wo, reflectance, wi, pdf, globals);
      break;
    default:
      // ERROR!
      return;
  }
}

void bsdf_get_emission(global bsdf_t *bsdf,
                       float3 *radiance,
                       global_state_t *globals) {
  switch (bsdf->type) {
    case BSDF_TYPE_DIFFUSE:
    case BSDF_TYPE_MIRROR:
    case BSDF_TYPE_MICROFACET:
    case BSDF_TYPE_GLASS:
      *radiance = (float3)(0, 0, 0);
      break;
    case BSDF_TYPE_EMISSION:
      *radiance = bsdf->u.emission.radiance;
      break;
    default:
      // ERROR!
      return;
  }
}

bool bsdf_is_delta(global bsdf_t *bsdf) {
  switch (bsdf->type) {
    case BSDF_TYPE_DIFFUSE:
    case BSDF_TYPE_MICROFACET:
    case BSDF_TYPE_EMISSION:
      return false;
    case BSDF_TYPE_GLASS:
    case BSDF_TYPE_MIRROR:
      return true;
    default:
      // ERROR!
      return false;
  }
}

#endif // KERNEL_BSDF_H

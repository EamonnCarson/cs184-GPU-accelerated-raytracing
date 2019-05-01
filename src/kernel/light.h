#ifndef KERNEL_LIGHT_H
#define KERNEL_LIGHT_H

#include "types.h"
#include "sampler.h"

bool light_is_delta(global light_t *light) {
  switch (light->type) {
    case LIGHT_TYPE_DIRECTIONAL:
    case LIGHT_TYPE_POINT:
      return true;
    case LIGHT_TYPE_HEMISPHERE:
    case LIGHT_TYPE_AREA:
      return false;
    default:
      // ERROR!
      return false;
  }
}

void directional_light_sample_l(global directional_light_t *light,
                                float3 *p,
                                float3 *radiance,
                                float3 *wi,
                                float *dist_to_light,
                                float *pdf,
                                global_state_t *globals) {
  *wi = light->dir_to_light;
  *dist_to_light = INFINITY;
  *pdf = 1.f;
  *radiance = light->radiance;
}

void hemisphere_light_sample_l(global hemisphere_light_t *light,
                               float3 *p,
                               float3 *radiance,
                               float3 *wi,
                               float *dist_to_light,
                               float *pdf,
                               global_state_t *globals) {
  float3 dir;
  sample_uniform_hemisphere(&dir, globals);
  *wi = dir.xzy;
  wi->y *= -1.f;
  *dist_to_light = INFINITY;
  *pdf = 1.f / (2.f * M_PI_F);
  *radiance = light->radiance;
}

void point_light_sample_l(global point_light_t *light,
                          float3 *p,
                          float3 *radiance,
                          float3 *wi,
                          float *dist_to_light,
                          float *pdf,
                          global_state_t *globals) {
  float3 d = light->position - *p;
  *wi = normalize(d);
  *dist_to_light = length(d);
  *pdf = 1.f;
  *radiance = light->radiance;
}

void area_light_sample_l(global area_light_t *light,
                         float3 *p,
                         float3 *radiance,
                         float3 *wi,
                         float *dist_to_light,
                         float *pdf,
                         global_state_t *globals) {
  float2 sample;
  sample_uniform_grid(&sample, globals);
  sample -= 0.5f;
  float3 d = light->position + sample.x * light->dim_x
             + sample.y * light->dim_y - *p;
  float dist = length(d);
  *wi = d / dist;
  float cos_theta = dot(*wi, light->direction);
  *dist_to_light = dist;
  *pdf = (dist * dist) / (length(light->dim_x) * length(light->dim_y) * fabs(cos_theta));
  *radiance = cos_theta < 0 ? light->radiance : (float3)(0, 0, 0);
}

void light_sample_l(global light_t *light,
                    float3 *p,
                    float3 *radiance,
                    float3 *wi,
                    float *dist_to_light,
                    float *pdf,
                    global_state_t *globals) {
  switch (light->type) {
    case LIGHT_TYPE_DIRECTIONAL:
      directional_light_sample_l(&light->u.directional,
                                 p,
                                 radiance,
                                 wi,
                                 dist_to_light,
                                 pdf,
                                 globals);
      break;
    case LIGHT_TYPE_HEMISPHERE:
      hemisphere_light_sample_l(&light->u.hemisphere,
                                p,
                                radiance,
                                wi,
                                dist_to_light,
                                pdf,
                                globals);
      break;
    case LIGHT_TYPE_POINT:
      point_light_sample_l(&light->u.point,
                           p,
                           radiance,
                           wi,
                           dist_to_light,
                           pdf,
                           globals);
      break;
    case LIGHT_TYPE_AREA:
      area_light_sample_l(&light->u.area,
                          p,
                          radiance,
                          wi,
                          dist_to_light,
                          pdf,
                          globals);
      break;
    default:
      // ERROR!
      return;
  }
}

#endif // KERNEL_LIGHT_H

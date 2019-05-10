#include "types.h"
#include "util.h"
#include "intersect.h"
#include "light.h"
#include "bsdf.h"

/* Path tracing functions */

void generate_ray(camera_t *camera, float cx, float cy, ray_t *output) {
  float3 dir = (float3)((2 * cx - 1) * tan(camera->h_fov * 0.5f),
                        (2 * cy - 1) * tan(camera->v_fov * 0.5f),
                        -1);
  dir = mat_mul(&camera->c2w, &dir);
  output->o = camera->pos;
  output->d = normalize(dir);
  output->min_t = camera->n_clip;
  output->max_t = camera->f_clip;
}

float3 zero_bounce_radiance(ray_t *ray,
                            intersection_t *isect,
                            global_state_t *globals) {
  float3 radiance;
  bsdf_get_emission(&globals->bsdfs[isect->bsdf_index],
                    &radiance,
                    globals);
  return radiance;
}

float3 est_radiance_global_illumination(ray_t *ray,
                                        global_state_t *globals) {
  intersection_t isect;
  float3 L_out = (float3)(0, 0, 0);
  if (!intersect_bvh(ray, globals->bvh, globals->primitives, &isect)) {
    return L_out;
  }

  L_out += zero_bounce_radiance(ray, &isect, globals);

  mat3_t o2w;
  make_coord_space(&isect.n, &o2w);
  mat3_t w2o = mat_transpose(&o2w);

  float3 hit_p = ray->o + ray->d * isect.t;
  float3 w_out = mat_mul(&w2o, &ray->d);
  w_out *= -1;

  for (uint light_idx = 0; light_idx < globals->light_count; light_idx++) {
    global light_t *light = &globals->lights[light_idx];
    uint light_samples = 1;
    if (!light_is_delta(light)) {
      light_samples = globals->light_samples;
    }

    float3 light_irradiance = (float3)(0, 0, 0);
    for (uint i = 0; i < light_samples; i++) {
      float3 w_in_world;
      float dist_to_light, pdf;
      float3 radiance;
      light_sample_l(light,
                     &hit_p,
                     &radiance,
                     &w_in_world,
                     &dist_to_light,
                     &pdf,
                     globals);
      float3 w_in = mat_mul(&w2o, &w_in_world);

      if (w_in.z < 0) {
        continue;
      }

      ray_t shadow = (ray_t) {
        hit_p + EPS_F * w_in_world,
        w_in_world,
        0.0,
        dist_to_light
      };
      if (intersect_bvh(&shadow, globals->bvh, globals->primitives, 0)) {
        continue;
      }

      float3 reflectance;
      bsdf_f(&globals->bsdfs[isect.bsdf_index],
             &w_out,
             &w_in,
             &reflectance,
             globals);
      light_irradiance += reflectance * radiance * fabs(w_in.z) / pdf;
    }
    L_out += light_irradiance / (float) light_samples;
  }

  return L_out;
}

kernel void
pathtrace_pixel(global float3 *output_image,
                uint2 dimensions,
                uint num_samples,
                uint light_samples,
                uint max_ray_depth,
                camera_t camera,
                global bvh_node_t *bvh,
                global primitive_t *primitives,
                global light_t *lights,
                uint light_count,
                global bsdf_t *bsdfs,
                local float3 *local_samples)
{
  uint x = get_global_id(0) + get_global_offset(0);
  uint y = get_global_id(1) + get_global_offset(1);
  uint z = get_global_id(2) + get_global_offset(2);

  rand_state_t rand_state = (y * get_global_size(0) + x) * get_global_size(2) + z;
  global_state_t globals = {
    &rand_state,
    light_samples,
    max_ray_depth,
    bvh,
    primitives,
    lights,
    light_count,
    bsdfs
  };

  ray_t ray;
  if (num_samples == 1) {
    generate_ray(&camera,
                 (x + 0.5f) / dimensions.x,
                 (y + 0.5f) / dimensions.y,
                 &ray);
  } else {
    generate_ray(&camera,
                 (x + rand(&rand_state)) / dimensions.x,
                 (y + rand(&rand_state)) / dimensions.y,
                 &ray);
  }
  float3 sample = est_radiance_global_illumination(&ray, &globals);

  // local float3 samples[get_local_size(1)][get_local_size(0)][get_local_size(2)];
  size_t lx = get_local_id(0);
  size_t ly = get_local_id(1);
  size_t lz = get_local_id(2);
  size_t local_sample_index = (ly * get_local_size(0) + lx) * get_local_size(2) + lz;
  local_samples[local_sample_index] = sample / num_samples;

  barrier(CLK_LOCAL_MEM_FENCE);

  if (x >= dimensions.x || y >= dimensions.y || z >= num_samples) {
    // We might have extra work units here since we need to evenly divide total
    // units with local units.
    return;
  }

  if (lz == 0) {
    float3 total_samples = (float3)(0, 0, 0);
    for (size_t sample = 0; sample < get_local_size(2); sample++) {
      if (lz + sample >= num_samples) {
        break;
      }
      total_samples += local_samples[local_sample_index + sample];
    }

    size_t max_sample_depth = (size_t) ceil((float) num_samples / get_local_size(2));
    size_t sample_depth = z / get_local_size(2);
    size_t output_index = (y * dimensions.x + x) * max_sample_depth + sample_depth;
    output_image[output_index] = clamp(total_samples,
                                       0.f,
                                       1.f);
  }
}

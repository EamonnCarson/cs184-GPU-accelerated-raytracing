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

float3 est_radiance_global_illumination(ray_t *ray,
                                        global_state_t *globals) {
  intersection_t isect;
  bool intersected = intersect_bvh(ray, globals->bvh, globals->primitives, &isect);
  if (intersected) {
    return (isect.n * 0.5f) + (float3)(0.5f, 0.5f, 0.5f);
  }
  return (float3)(0, 0, 0);
}

kernel void
pathtrace_pixel(global float4 *output_image,
                uint2 dimensions,
                uint num_samples,
                uint light_samples,
                uint max_ray_depth,
                camera_t camera,
                global bvh_node_t *bvh,
                global primitive_t *primitives,
                global light_t *lights,
                uint light_count,
                global bsdf_t *bsdfs)
{
  uint x = get_global_id(0);
  uint y = get_global_id(1);
  if (x >= dimensions.x || y >= dimensions.y) {
    // We might have extra work units here since we need to evenly divide total
    // units with local units.
    return;
  }

  rand_state_t rand_state = y * get_global_size(0) + x;
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

  float3 total = (float3)(0, 0, 0);
  for (int i = 0; i < num_samples; i++) {
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
    total += est_radiance_global_illumination(&ray, &globals);
  }
  output_image[y * dimensions.x + x] = (float4)(total / num_samples, 1);
}

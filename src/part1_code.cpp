//
// TODO: Copy over 3-1 code after turning on BUILD_3-1 flag
//

#include "part1_code.h"
#include <time.h>

using namespace CGL::StaticScene;

using std::min;
using std::max;

namespace CGL {

  Spectrum PathTracer::estimate_direct_lighting_hemisphere(const Ray& r, const Intersection& isect) {
    // Estimate the lighting from this intersection coming directly from a light.
    // For this function, sample uniformly in a hemisphere.

    // make a coordinate system for a hit point
    // with N aligned with the Z direction.
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    // w_out points towards the source of the ray (e.g.,
    // toward the camera if this is a primary ray)
    const Vector3D& hit_p = r.o + r.d * isect.t;
    const Vector3D& w_out = w2o * (-r.d);

    // This is the same number of total samples as estimate_direct_lighting_importance (outside of delta lights).
    // We keep the same number of samples for clarity of comparison.
    int num_samples = scene->lights.size() * ns_area_light;
    Spectrum L_out;

    // TODO (Part 3.2):
    // Write your sampling loop here
    // COMMENT OUT `normal_shading` IN `est_radiance_global_illumination` BEFORE YOU BEGIN
    for (int i = 0; i < num_samples; i++) {
      const Vector3D w_in = hemisphereSampler->get_sample();
      const Vector3D& w_in_world = o2w * w_in;
      Intersection cast_isect;
      Ray cast(hit_p + EPS_D * w_in_world, w_in_world);
      if (bvh->intersect(cast, &cast_isect)) {
        L_out += isect.bsdf->f(w_out, w_in) * cast_isect.bsdf->get_emission() * w_in.z;
      }
    }

    return L_out * (2.0 * PI) / num_samples;
  }

  Spectrum PathTracer::estimate_direct_lighting_importance(const Ray& r, const Intersection& isect) {
    // Estimate the lighting from this intersection coming directly from a light.
    // To implement importance sampling, sample only from lights, not uniformly in a hemisphere.

    // make a coordinate system for a hit point
    // with N aligned with the Z direction.
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    // w_out points towards the source of the ray (e.g.,
    // toward the camera if this is a primary ray)
    const Vector3D& hit_p = r.o + r.d * isect.t;
    const Vector3D& w_out = w2o * (-r.d);
    Spectrum L_out;

    // TODO (Part 3.2):
    // Here is where your code for looping over scene lights goes
    // COMMENT OUT `normal_shading` IN `est_radiance_global_illumination` BEFORE YOU BEGIN
    for (SceneLight *light : scene->lights) {
      size_t light_samples = 1;
      if (!light->is_delta_light()) {
        light_samples = ns_area_light;
      }

      Spectrum light_irradiance;
      for (size_t i = 0; i < light_samples; i++) {
        Vector3D w_in_world;
        float dist_to_light, pdf;
        Spectrum radiance = light->sample_L(hit_p, &w_in_world, &dist_to_light, &pdf);
        const Vector3D& w_in = w2o * w_in_world;

        if (w_in.z < 0) {
          continue;
        }

        Ray cast(hit_p + EPS_D * w_in_world, w_in_world, dist_to_light);
        if (bvh->intersect(cast)) {
          continue;
        }

        light_irradiance += isect.bsdf->f(w_out, w_in) * radiance * fabs(w_in.z) / pdf;
      }
      L_out += light_irradiance / (double) light_samples;
    }

    return L_out;
  }

  Spectrum PathTracer::zero_bounce_radiance(const Ray&r, const Intersection& isect) {
    // TODO (Part 4.2):
    // Returns the light that results from no bounces of light

    return isect.bsdf->get_emission();
  }

  Spectrum PathTracer::one_bounce_radiance(const Ray&r, const Intersection& isect) {
    if (direct_hemisphere_sample) {
      return estimate_direct_lighting_hemisphere(r, isect);
    } else {
      return estimate_direct_lighting_importance(r, isect);
    }
  }

  Spectrum PathTracer::at_least_one_bounce_radiance(const Ray&r, const Intersection& isect) {
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    Vector3D hit_p = r.o + r.d * isect.t;
    Vector3D w_out = w2o * (-r.d);

    Spectrum L_out;
    if (!isect.bsdf->is_delta()) {
      L_out += one_bounce_radiance(r, isect);
    }

    // TODO (Part 4.2):
    // Here is where your code for sampling the BSDF,
    // performing Russian roulette step, and returning a recursively
    // traced ray (when applicable) goes
    Vector3D w_in;
    float pdf;
    Spectrum radiance = isect.bsdf->sample_f(w_out, &w_in, &pdf);

    double continuation_probability = 0.7;
    if (max_ray_depth > 1 && r.depth == max_ray_depth) {
      continuation_probability = 1.0;
    }
    if (r.depth > 1 && coin_flip(continuation_probability) && pdf > 0) {
      // Continue this ray
      const Vector3D& w_in_world = o2w * w_in;
      Ray new_ray(hit_p + EPS_D * w_in_world, w_in_world, (int) r.depth - 1);
      Intersection new_isect;
      if (bvh->intersect(new_ray, &new_isect)) {
        Spectrum L = at_least_one_bounce_radiance(new_ray, new_isect);
        if (isect.bsdf->is_delta()) {
          L += zero_bounce_radiance(new_ray, new_isect);
        }
        L_out += L * radiance * fabs(w_in.z) / pdf / continuation_probability;
      }
    }

    return L_out;
  }

  Spectrum PathTracer::est_radiance_global_illumination(const Ray &r) {
    Intersection isect;
    Spectrum L_out;

    // You will extend this in assignment 3-2.
    // If no intersection occurs, we simply return black.
    // This changes if you implement hemispherical lighting for extra credit.

    if (!bvh->intersect(r, &isect))
      return envLight ? envLight->sample_dir(r) : L_out;

    // This line returns a color depending only on the normal vector
    // to the surface at the intersection point.
    // REMOVE IT when you are ready to begin Part 3.

    // return normal_shading(isect.n);

    // TODO (Part 3): Return the direct illumination.
    // return estimate_direct_lighting_hemisphere(r, isect);
    // return estimate_direct_lighting_importance(r, isect);

    // TODO (Part 4): Accumulate the "direct" and "indirect"
    // parts of global illumination into L_out rather than just direct
    L_out += zero_bounce_radiance(r, isect);
    if (max_ray_depth > 0) {
      L_out += at_least_one_bounce_radiance(r, isect);
    }

    return L_out;
  }

  Spectrum PathTracer::raytrace_pixel(size_t x, size_t y, bool useThinLens) {
    int max_samples = ns_aa;            // total samples to evaluate
    Vector2D origin = Vector2D(x,y);    // bottom left corner of the pixel

    const static bool adaptive_sampling = true;
    if (adaptive_sampling) {
      double i = 0.0;
      double s1 = 0.0, s2 = 0.0;

      int num_samples = 0;
      Spectrum total;
      while (num_samples < max_samples) {
        num_samples++;
        Vector2D sample_offset = gridSampler->get_sample();
        if (max_samples == 1) {
          sample_offset = {0.5, 0.5};
        }
        Vector2D lensSample = gridSampler->get_sample();
        // Ray r = camera->generate_ray((origin.x + sample_offset.x) / sampleBuffer.w,
        //                              (origin.y + sample_offset.y) / sampleBuffer.h);
        Ray r = camera->generate_ray_for_thin_lens((origin.x + sample_offset.x) / sampleBuffer.w,
                                                   (origin.y + sample_offset.y) / sampleBuffer.h,
                                                   lensSample[0],
                                                   lensSample[1] * 2.0 * PI);
        r.depth = max_ray_depth;
        Spectrum sample = est_radiance_global_illumination(r);
        total += sample;
        const float& illum = sample.illum();
        s1 += illum;
        s2 += illum * illum;

        // Check for convergence
        if (num_samples % samplesPerBatch == 0) {
          double mean = s1 / num_samples;
          double var = (1.0 / (num_samples - 1.0)) * (s2 - (s1 * s1 / num_samples));
          if (1.96 * sqrt(var) / sqrt(num_samples) <= maxTolerance * mean) {
            // Converged!
            break;
          }
        }
      }

      sampleCountBuffer[x + y * frameBuffer.w] = num_samples;
      return total / num_samples;
    } else {
      vector<Vector2D> sample_offsets(max_samples);
      if (max_samples == 1) {
        sample_offsets[0] = {0.5, 0.5};
      } else {
        for (int i = 0; i < max_samples; i++) {
          sample_offsets[i] = gridSampler->get_sample();
        }
      }

      Spectrum total;
      for (int i = 0; i < max_samples; i++) {
        Ray r = camera->generate_ray((origin.x + sample_offsets[i].x) / sampleBuffer.w,
                                     (origin.y + sample_offsets[i].y) / sampleBuffer.h);
        r.depth = max_ray_depth;
        total += est_radiance_global_illumination(r);
      }

      sampleCountBuffer[x + y * frameBuffer.w] = max_samples;
      return total / max_samples;
    }
  }

  // Diffuse BSDF //

  Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
    return reflectance / PI;
  }

  Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
    *wi = sampler.get_sample(pdf);
    return reflectance / PI;
  }

  // Camera //
  Ray Camera::generate_ray(double x, double y) const {
    Vector3D sensor_bot_left(-tan(radians(hFov)*.5), -tan(radians(vFov)*.5), -1),
        sensor_top_right(tan(radians(hFov)*.5), tan(radians(vFov)*.5), -1);

    Vector3D direction((sensor_top_right.x - sensor_bot_left.x) * x + sensor_bot_left.x,
                       (sensor_top_right.y - sensor_bot_left.y) * y + sensor_bot_left.y,
                       -1);
    direction = c2w * direction;
    direction.normalize();
    Ray ray(pos, direction);
    ray.min_t = nClip;
    ray.max_t = fClip;
    return ray;
  }
}

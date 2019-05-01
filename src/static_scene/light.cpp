#include "light.h"

#include <iostream>

#include "../sampler.h"

namespace CGL { namespace StaticScene {

// Directional Light //

DirectionalLight::DirectionalLight(const Spectrum& rad,
                                   const Vector3D& lightDir)
    : radiance(rad) {
  dirToLight = -lightDir.unit();
}

Spectrum DirectionalLight::sample_L(const Vector3D& p, Vector3D* wi,
                                    float* distToLight, float* pdf) const {
  *wi = dirToLight;
  *distToLight = INF_D;
  *pdf = 1.0;
  return radiance;
}

void DirectionalLight::kernel_struct(kernel_light_t *kernel_light) {
  kernel_light->type = KERNEL_LIGHT_TYPE_DIRECTIONAL;
  kernel_directional_light_t *light = &kernel_light->u.directional;
  light->radiance = cglSpectrumToKernel(radiance);
  light->dir_to_light = cglVectorToKernel(dirToLight);
}

// Infinite Hemisphere Light //

InfiniteHemisphereLight::InfiniteHemisphereLight(const Spectrum& rad)
    : radiance(rad) {
  sampleToWorld[0] = Vector3D(1,  0,  0);
  sampleToWorld[1] = Vector3D(0,  0, -1);
  sampleToWorld[2] = Vector3D(0,  1,  0);
}

Spectrum InfiniteHemisphereLight::sample_L(const Vector3D& p, Vector3D* wi,
                                           float* distToLight,
                                           float* pdf) const {
  Vector3D dir = sampler.get_sample();
  *wi = sampleToWorld* dir;
  *distToLight = INF_D;
  *pdf = 1.0 / (2.0 * PI);
  return radiance;
}

void InfiniteHemisphereLight::kernel_struct(kernel_light_t *kernel_light) {
  kernel_light->type = KERNEL_LIGHT_TYPE_HEMISPHERE;
  kernel_hemisphere_light_t *light = &kernel_light->u.hemisphere;
  light->radiance = cglSpectrumToKernel(radiance);
}

// Point Light //

PointLight::PointLight(const Spectrum& rad, const Vector3D& pos) :
  radiance(rad), position(pos) { }

Spectrum PointLight::sample_L(const Vector3D& p, Vector3D* wi,
                             float* distToLight,
                             float* pdf) const {
  Vector3D d = position - p;
  *wi = d.unit();
  *distToLight = d.norm();
  *pdf = 1.0;
  return radiance;
}

void PointLight::kernel_struct(kernel_light_t *kernel_light) {
  kernel_light->type = KERNEL_LIGHT_TYPE_POINT;
  kernel_point_light_t *light = &kernel_light->u.point;
  light->radiance = cglSpectrumToKernel(radiance);
  light->position = cglVectorToKernel(position);
}

// Spot Light //

SpotLight::SpotLight(const Spectrum& rad, const Vector3D& pos,
                     const Vector3D& dir, float angle) {

}

Spectrum SpotLight::sample_L(const Vector3D& p, Vector3D* wi,
                             float* distToLight, float* pdf) const {
  return Spectrum();
}


// Area Light //

AreaLight::AreaLight(const Spectrum& rad, 
                     const Vector3D& pos,   const Vector3D& dir, 
                     const Vector3D& dim_x, const Vector3D& dim_y)
  : radiance(rad), position(pos), direction(dir),
    dim_x(dim_x), dim_y(dim_y), area(dim_x.norm() * dim_y.norm()) { }

Spectrum AreaLight::sample_L(const Vector3D& p, Vector3D* wi, 
                             float* distToLight, float* pdf) const {

  Vector2D sample = sampler.get_sample() - Vector2D(0.5f, 0.5f);
  Vector3D d = position + sample.x * dim_x + sample.y * dim_y - p;
  float sqDist = d.norm2();
  float dist = sqrt(sqDist);
  *wi = d / dist;
  float cosTheta = dot(*wi, direction);
  *distToLight = dist;
  *pdf = sqDist / (area * fabs(cosTheta));
  return cosTheta < 0 ? radiance : Spectrum();
};

void AreaLight::kernel_struct(kernel_light_t *kernel_light) {
  kernel_light->type = KERNEL_LIGHT_TYPE_AREA;
  kernel_area_light_t *light = &kernel_light->u.area;
  light->radiance = cglSpectrumToKernel(radiance);
  light->position = cglVectorToKernel(position);
  light->direction = cglVectorToKernel(direction, true);
  light->dim_x = cglVectorToKernel(dim_x);
  light->dim_y = cglVectorToKernel(dim_y);
}

// Sphere Light //

SphereLight::SphereLight(const Spectrum& rad, const SphereObject* sphere) {

}

Spectrum SphereLight::sample_L(const Vector3D& p, Vector3D* wi, 
                               float* distToLight, float* pdf) const {

  return Spectrum();
}

// Mesh Light

MeshLight::MeshLight(const Spectrum& rad, const Mesh* mesh) {

}

Spectrum MeshLight::sample_L(const Vector3D& p, Vector3D* wi, 
                             float* distToLight, float* pdf) const {
  return Spectrum();
}

} // namespace StaticScene
} // namespace CGL

#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CGL {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {
  Vector3D z = Vector3D(n.x, n.y, n.z);
  Vector3D h = z;
  if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
  else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
  else h.z = 1.0;

  z.normalize();
  Vector3D y = cross(h, z);
  y.normalize();
  Vector3D x = cross(z, y);
  x.normalize();

  o2w[0] = x;
  o2w[1] = y;
  o2w[2] = z;
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 1.2
  // Using BSDF::reflect(), implement sample_f for a mirror surface
  *pdf = 1.f;
  reflect(wo, wi);
  return reflectance / abs_cos_theta(*wi);
}

void MirrorBSDF::kernel_struct(kernel_bsdf_t *kernel_bsdf) {
  kernel_bsdf->type = KERNEL_BSDF_TYPE_MIRROR;
  kernel_bsdf->u.mirror.reflectance = cglSpectrumToKernel(reflectance);
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
    return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
  // TODO: 2.2
  // Compute Beckmann normal distribution function (NDF) here.
  // You will need the roughness alpha.
  float theta_h = getTheta(h);
  return expf(-powf(tanf(theta_h), 2) / (alpha * alpha))
      / (PI * alpha * alpha * powf(cosf(theta_h), 4));
}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
  // TODO: 2.3
  // Compute Fresnel term for reflection on dielectric-conductor interface.
  // You will need both eta and etaK, both of which are Spectrum.
  Spectrum n2_k2 = (eta * eta + k * k);
  Spectrum ncos_2 = 2 * eta * fabs(wi.z);
  float cos2 = wi.z * wi.z;
  Spectrum r_s  = (n2_k2 - ncos_2 + cos2)
                  / (n2_k2 + ncos_2 + cos2);
  Spectrum r_p = (n2_k2 * cos2 - ncos_2 + 1)
                 / (n2_k2 * cos2 + ncos_2 + 1);
  return (r_s + r_p) / 2.f;
}

Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // TODO: 2.1
  // Implement microfacet model here

  if (!(wo.z > 0 && wi.z > 0)) {
    return Spectrum();
  }
  Vector3D h = wo + wi;
  h.normalize();
  static const Vector3D n = {0, 0, 1};
  return (F(wi) * G(wo, wi) * D(h)) / (4 * dot(n, wo) * dot(n, wi));
}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 2.4
  // *Importance* sample Beckmann normal distribution function (NDF) here.
  // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
  //       and return the sampled BRDF value.
  // *wi = cosineHemisphereSampler.get_sample(pdf); //placeholder
  // return MicrofacetBSDF::f(wo, *wi);

  Vector2D r = sampler.get_sample();
  float theta = atanf(sqrtf(-(alpha * alpha) * logf(1 - r[0])));
  float phi = 2 * PI * r[1];
  Vector3D h = {cosf(phi) * sinf(theta), sinf(phi) * sinf(theta), cosf(theta)};
  *wi = -wo + 2 * dot(wo, h) * h;
  if (wi->z <= 0) {
    *pdf = 0;
    return Spectrum();
  }

  float p_theta = (2 * sinf(theta)) / (alpha * alpha * powf(cosf(theta), 3))
                  * expf(-powf(tanf(theta), 2) / (alpha * alpha));
  float p_phi = 1 / (2 * PI);
  float p_h = p_theta * p_phi / sinf(theta);
  *pdf = p_h / (4 * dot(*wi, h));
  return f(wo, *wi);
}

void MicrofacetBSDF::kernel_struct(kernel_bsdf_t *kernel_bsdf) {
  kernel_bsdf->type = KERNEL_BSDF_TYPE_MICROFACET;
  kernel_microfacet_bsdf_t *microfacet = &kernel_bsdf->u.microfacet;
  microfacet->eta = cglSpectrumToKernel(eta);
  microfacet->k = cglSpectrumToKernel(k);
  microfacet->alpha = alpha;
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO: 1.4
  // Compute Fresnel coefficient and either reflect or refract based on it.
  if (!refract(wo, wi, ior)) {
    reflect(wo, wi);
    *pdf = 1.f;
    return reflectance / abs_cos_theta(*wi);
  } else {
    float r0 = powf((1 - ior) / (1 + ior), 2);
    float r = r0 + (1.f - r0) * powf(1 - fabs(wo.z), 5);
    if (coin_flip(r)) {
      reflect(wo, wi);
      *pdf = 1.f;
      return r * reflectance / abs_cos_theta(*wi);
    } else {
      *pdf = 1.f - r;
      float eta = wo.z < 0 ? ior : 1.f / ior;
      return (1 - r) * transmittance / abs_cos_theta(*wi) / (eta * eta);
    }
  }
  return Spectrum();
}

void GlassBSDF::kernel_struct(kernel_bsdf_t *kernel_bsdf) {
  kernel_bsdf->type = KERNEL_BSDF_TYPE_GLASS;
  kernel_glass_bsdf_t *glass = &kernel_bsdf->u.glass;
  glass->ior = ior;
  glass->reflectance = cglSpectrumToKernel(reflectance);
  glass->transmittance = cglSpectrumToKernel(transmittance);
}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {

  // TODO: 1.1
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  *wi = {-wo.x, -wo.y, wo.z};
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {

  // TODO: 1.3
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.

  float eta;
  int z_mult;
  if (wo.z < 0) { // exiting
    z_mult = 1;
    eta = ior;
  } else { // entering
    z_mult = -1;
    eta = 1.f / ior;
  }

  float z_sq = 1 - (eta * eta) * (1 - (wo.z * wo.z));
  if (z_sq < 0) {
    return false; // Total internal reflection
  }
  *wi = {-eta * wo.x, -eta  * wo.y, z_mult * sqrtf(z_sq)};
  return true;
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0 / PI;
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

void EmissionBSDF::kernel_struct(kernel_bsdf_t *kernel_bsdf) {
  kernel_bsdf->type = KERNEL_BSDF_TYPE_EMISSION;
  kernel_bsdf->u.emission.radiance = cglSpectrumToKernel(radiance);
}

// Diffuse BSDF //

void DiffuseBSDF::kernel_struct(kernel_bsdf_t *kernel_bsdf) {
  kernel_bsdf->type = KERNEL_BSDF_TYPE_DIFFUSE;
  kernel_bsdf->u.diffuse.reflectance = cglSpectrumToKernel(reflectance);
}

} // namespace CGL

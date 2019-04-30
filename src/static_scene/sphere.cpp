#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  double a = dot(r.d, r.d);
  double b = dot(2 * (r.o - o), r.d);
  double c = dot(r.o - o, r.o - o) - r2;
  double det = b * b - 4 * a * c;
  if (det < 0) {
    return false;
  }

  double sqrt_det = sqrt(det);
  t1 = (-b - sqrt_det) / (2 * a);
  t2 = (-b + sqrt_det) / (2 * a);

  return true;
}

bool Sphere::intersect(const Ray& r) const {
  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.

  return intersect(r, NULL);
}

bool Sphere::intersect(const Ray& r, Intersection *i) const {
  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.

  double t1, t2;
  if (test(r, t1, t2)) {
    if (t1 < 0) {
      t1 = t2;
    }
    if (t1 < r.min_t || t1 > r.max_t) {
      return false;
    }
    if (i) {
      i->t = t1;
      i->primitive = this;
      Vector3D normal = (r.o + r.d * t1) - o;
      normal.normalize();
      i->n = normal;
      i->bsdf = get_bsdf();
    }
    r.max_t = t1;
    return true;
  }
  return false;
}

void Sphere::draw(const Color& c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c, float alpha) const {
    //Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::kernel_struct(kernel_primitive_t *kernel_primitive) const {
  kernel_primitive->type = KERNEL_PRIMITIVE_TYPE_SPHERE;
}


} // namespace StaticScene
} // namespace CGL

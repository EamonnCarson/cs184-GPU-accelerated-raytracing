#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL { namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
    mesh(mesh), v1(v1), v2(v2), v3(v3) { }

BBox Triangle::get_bbox() const {

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  BBox bb(p1);
  bb.expand(p2);
  bb.expand(p3);
  return bb;

}

bool Triangle::intersect(const Ray& r) const {

  // TODO (Part 1.3):
  // implement ray-triangle intersection
  return intersect(r, NULL);
}

bool Triangle::intersect(const Ray& r, Intersection *isect) const {

  // TODO (Part 1.3):
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  Vector3D n1(mesh->normals[v1]), n2(mesh->normals[v2]), n3(mesh->normals[v3]);

  Vector3D p1p2 = p2 - p1,
           p1p3 = p3 - p1,
           pvec = cross(r.d, p1p3);
  double det = dot(p1p2, pvec);
  if (abs(det) <= 0) {
    return false;
  }
  double invDet = 1 / det;
  Vector3D tvec = r.o - p1;
  double u = dot(tvec, pvec) * invDet;
  if (u < 0 || u > 1) {
    return false;
  }

  Vector3D qvec = cross(tvec, p1p2);
  double v = dot(r.d, qvec) * invDet;
  if (v < 0 || u + v > 1) {
    return false;
  }

  double t = dot(p1p3, qvec) * invDet;
  if (t < r.min_t || t > r.max_t) {
    return false;
  }

  if (isect) {
    isect->t = t;
    isect->primitive = this;
    isect->n = (1.0 - u - v) * n1 + u * n2 + v * n3;
    isect->bsdf = get_bsdf();
  }

  r.max_t = t;
  return true;
}

void Triangle::draw(const Color& c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::kernel_struct(kernel_primitive_t *kernel_primitive,
                             std::vector<BSDF*>& bsdf_pointers) {
  kernel_primitive->type = KERNEL_PRIMITIVE_TYPE_TRIANGLE;
  kernel_triangle_t *kernel_triangle = &kernel_primitive->triangle;
  kernel_triangle->vertices[0] = cglVectorToKernel(mesh->positions[v1]);
  kernel_triangle->vertices[1] = cglVectorToKernel(mesh->positions[v2]);
  kernel_triangle->vertices[2] = cglVectorToKernel(mesh->positions[v3]);
  kernel_triangle->normals[0] = cglVectorToKernel(mesh->normals[v1], true);
  kernel_triangle->normals[1] = cglVectorToKernel(mesh->normals[v2], true);
  kernel_triangle->normals[2] = cglVectorToKernel(mesh->normals[v3], true);

  uint32_t bsdf_index = distance(bsdf_pointers.begin(),
                                 find(bsdf_pointers.begin(), bsdf_pointers.end(), get_bsdf()));
  if (bsdf_index > bsdf_pointers.size() || bsdf_index < 0) {
    cerr << "[Pathtracer] Error with BSDF -> device conversion" << endl;
    throw;
  } else if (bsdf_index == bsdf_pointers.size()) {
    bsdf_pointers.push_back(get_bsdf());
  }
  kernel_triangle->bsdf_index = bsdf_index;
}


} // namespace StaticScene
} // namespace CGL

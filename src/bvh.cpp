#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c, alpha);
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c, alpha);
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.
  BBox total_bbox, bb;
  for (Primitive *p : prims) {
    bb = p->get_bbox();
    total_bbox.expand(bb);
  }
  BVHNode *node = new BVHNode(total_bbox);
  if (prims.size() <= max_leaf_size) {
    node->prims = new vector<Primitive *>(prims);
    return node;
  }

  // Use SAH to partition
  static const uint16_t num_buckets = 8;
  uint8_t best_axis = 0;
  uint16_t best_split = num_buckets / 2;
  double best_cost = INFINITY;
  BBox buckets[3][num_buckets];
  uint32_t tri_count[3][num_buckets];

  for (uint8_t axis = 0; axis < 3; axis++) {
    // Initialize partitions
    double total_width = total_bbox.max[axis] - total_bbox.min[axis];
    for (uint16_t i = 0; i < num_buckets; i++) {
      tri_count[axis][i] = 0;
    }

    // Compute partitions
    for (Primitive *p : prims) {
      bb = p->get_bbox();
      uint16_t bucket = floor(((bb.centroid()[axis] - total_bbox.min[axis])
                               / total_width) * num_buckets);
      buckets[axis][bucket].expand(bb);
      tri_count[axis][bucket]++;
    }

    // Find best partitioning
    for (uint16_t split = 1; split < num_buckets; split++) {
      BBox left_bbox, right_bbox;
      uint32_t left_count = 0, right_count = 0;
      for (uint16_t bucket = 0; bucket < num_buckets; bucket++) {
        if (bucket < split) {
          left_bbox.expand(buckets[axis][bucket]);
          left_count += tri_count[axis][bucket];
        } else {
          right_bbox.expand(buckets[axis][bucket]);
          right_count += tri_count[axis][bucket];
        }
      }

      double cost = left_bbox.volume() * left_count +
                    right_bbox.volume() * right_count;
      if (left_count > 0 && right_count > 0 && cost < best_cost) {
        best_cost = cost;
        best_axis = axis;
        best_split = split;
      }
    }
  }

  vector<Primitive*> left_prims, right_prims;

  if (best_cost == INFINITY) {
    // Edge case, all the primitives fall into the same bucket (or share the
    // same centroid)
    for (size_t i = 0; i < prims.size(); i++) {
      if (i < prims.size() / 2) {
        left_prims.push_back(prims[i]);
      } else {
        right_prims.push_back(prims[i]);
      }
    }
  } else {
    double split_ratio = ((double) best_split) / ((double) num_buckets);
    for (Primitive *p : prims) {
      if (((p->get_bbox().centroid()[best_axis] - total_bbox.min[best_axis]) /
           (total_bbox.max[best_axis] - total_bbox.min[best_axis])) < split_ratio) {
        left_prims.push_back(p);
      } else {
        right_prims.push_back(p);
      }
    }
  }
  node->l = construct_bvh(left_prims, max_leaf_size);
  node->r = construct_bvh(right_prims, max_leaf_size);
  return node;
}


bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {

  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.
  double t0, t1;
  total_isects++;
  if (!node->bb.intersect(ray, t0, t1)) {
    return false;
  }
  if (t0 > ray.max_t || t1 < ray.min_t) {
    return false;
  }

  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray)) {
        return true;
      }
    }
    return false;
  }
  return intersect(ray, node->l) || intersect(ray, node->r);
}

bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {

  // TODO (Part 2.3):
  // Fill in the intersect function.
  double t0, t1;
  total_isects++;
  if (!node->bb.intersect(ray, t0, t1)) {
    return false;
  }
  if (t0 > ray.max_t || t1 < ray.min_t) {
    return false;
  }

  bool intersects = false;
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray, i)) {
        intersects = true;
      }
    }
    return intersects;
  }
  intersects |= intersect(ray, i, node->l);
  intersects |= intersect(ray, i, node->r);
  return intersects;
}

}  // namespace StaticScene
}  // namespace CGL

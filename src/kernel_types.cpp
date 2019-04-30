#include "kernel_types.h"

cl_float3 cglVectorToKernel(CGL::Vector3D vector, bool normalize) {
  cl_float3 out = {(float) vector.x, (float) vector.y, (float) vector.z};
  if (normalize) {
    cl_float sum = fabs(out.s0 + out.s1 + out.s2);
    out.s0 /= sum;
    out.s1 /= sum;
    out.s2 /= sum;
  }
  return out;
}

kernel_mat3 cglMatrixToKernel(CGL::Matrix3x3 matrix) {
  return {
    {(float) matrix[0][0], (float) matrix[0][1], (float) matrix[0][2]},
    {(float) matrix[1][0], (float) matrix[1][1], (float) matrix[1][2]},
    {(float) matrix[2][0], (float) matrix[2][1], (float) matrix[2][2]}
  };
}

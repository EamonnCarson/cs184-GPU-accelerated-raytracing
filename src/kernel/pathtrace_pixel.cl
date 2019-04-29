kernel void
pathtracePixel(global float4 *outputImage, ulong2 dimensions)
{
  uint x = get_global_id(0);
  uint y = get_global_id(1);
  outputImage[y * dimensions.x + x] = (float4)((float) x / dimensions.x,
                                               (float) y / dimensions.y,
                                               0, 1);
}

#include <metal_stdlib>
using namespace metal;
kernel float k(device float *a [[buffer(0)]]) { return a[0]; }
kernel void k2(float *a [[buffer(0)]], uint i [[buffer]]) { }
kernel void k3(device float *a [[buffer(0), texture(1, 2)]], uint i [[thread_position_in_grid(3)]]) { }

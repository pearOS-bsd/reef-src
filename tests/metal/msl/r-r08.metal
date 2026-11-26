#include <metal_stdlib>
using namespace metal;
#define BODY { threadgroup float z[4]; z[0] = 1; o[i] = z[0]; }
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
BODY

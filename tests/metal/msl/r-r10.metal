threadgroup int at_start;
#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = 0; }

#include <metal_stdlib>
using namespace metal;
constant float a = sin(0.5f);
constant float b = a * 2 + cos(a);
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = a + b; }

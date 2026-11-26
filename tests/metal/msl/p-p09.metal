#include <metal_stdlib>
using namespace metal;
constant float a = sin(0.5f);
constant float unused = cos(0.5f);
constexpr constant float c = 3;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = c; }

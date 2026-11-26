#include <metal_stdlib>
using namespace metal;
float recurse(float x) { return x > 1 ? recurse(x / 2) : x; }
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = recurse(o[i]); }

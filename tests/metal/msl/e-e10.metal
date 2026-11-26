#include <metal_stdlib>
using namespace metal;
float helper(float x);
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = helper(o[i]); }

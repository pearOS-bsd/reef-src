#include <metal_stdlib>
using namespace metal;
constant float scale [[function_constant(0)]];
constant float derived = is_function_constant_defined(scale) ? scale * 2 : sin(1.0f);
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = derived; }

#include <metal_stdlib>
using namespace metal;
constant float4 table[3] = {float4(sin(1.0f)), float4(cos(1.0f), 0, 1, 2), float4(1, 2, 3, exp(1.0f))};
kernel void k(device float4 *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = table[i % 3] + table[0]; }

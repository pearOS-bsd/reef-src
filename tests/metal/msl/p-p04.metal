#include <metal_stdlib>
using namespace metal;
constant float4 v = float4(sin(1.0f), 2, 3, 4);
constant float3 w = float3(v.x, log(2.0f), 0);
constant float2x2 m = float2x2(float2(sin(0.25f), 1), float2(0, cos(0.25f)));
kernel void k(device float4 *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = v + float4(w, 1) + float4(m[0], m[1]); }

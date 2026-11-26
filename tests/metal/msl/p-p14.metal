#include <metal_stdlib>
using namespace metal;
struct S { float4 v; S(float s) { v.x = sin(s); v.y = s; v.z = s * 2; v.w = 1; } };
constant S s1(0.5f);
kernel void k(device float4 *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = s1.v; }

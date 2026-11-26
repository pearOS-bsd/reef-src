#include <metal_stdlib>
using namespace metal;
struct V { float4 v; V() { for (int i = 0; i < 4; i++) v[i] = i * 2 + 1; } };
constant V g;
kernel void k(device float4 *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = g.v; }

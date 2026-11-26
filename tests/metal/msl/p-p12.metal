#include <metal_stdlib>
using namespace metal;
struct W { int a; float3 v[2]; W() : a(3) { v[0].x = 1; v[0].y = 2; v[0].z = 3; v[1].y = float(a) * 2; v[1].x = 0; v[1].z = 5; } };
constant W w;
kernel void k(device float3 *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = w.v[i & 1] + float(w.a); }

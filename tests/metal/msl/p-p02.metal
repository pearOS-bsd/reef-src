#include <metal_stdlib>
using namespace metal;
struct P { float x; float y; P(float v) : x(v * 2), y(sqrt(v)) {} };
constant P p(3.0f);
constant P q(p.x + 1);
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = p.x + p.y + q.y; }
vertex float4 v(uint id [[vertex_id]]) { return float4(p.x, q.x, float(id), 1); }

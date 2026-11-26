#include <metal_stdlib>
using namespace metal;
[[visible]] float helper(float x, int y) { return x * y; }
[[visible]] float other(float x);
fragment float4 f(float4 p [[position]]) { return float4(helper(p.x, 2)); }

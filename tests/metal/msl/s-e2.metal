#include <metal_stdlib>
using namespace metal;
struct O { float4 p; };
vertex O v(uint id [[vertex_id]]) { O o; o.p = float4(id); return o; }
fragment float4 f(float4 p [[position]], texture2d<float> t [[texture(-1)]]) { return p + undefined_thing; }

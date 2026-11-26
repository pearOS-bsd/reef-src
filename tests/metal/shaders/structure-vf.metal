#include <metal_stdlib>
using namespace metal;
struct VOut { float4 pos [[position]]; float2 uv; int flag; float size [[point_size]]; };
vertex VOut v(uint vid [[vertex_id]], constant float4 *p [[buffer(0)]])
{
    VOut o;
    o.pos = p[vid];
    o.uv = p[vid].xy;
    o.flag = int(vid);
    o.size = 1.0;
    return o;
}

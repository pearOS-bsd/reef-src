#include <metal_stdlib>
using namespace metal;
struct VIn { float3 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };
struct VOut { float4 pos [[position]]; float2 uv; };
struct U { float4x4 mvp; };
vertex VOut vs(VIn in [[stage_in]], constant U &u [[buffer(1)]], uint vid [[vertex_id]])
{
    VOut o; o.pos = u.mvp * float4(in.pos, 1); o.uv = in.uv + float(vid); return o;
}
fragment float4 fs(VOut in [[stage_in]], texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]])
{
    return t.sample(s, in.uv) * sin(in.uv.x);
}
kernel void ks(device float *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = sqrt(a[i]); }

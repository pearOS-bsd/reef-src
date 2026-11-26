//opt -nofast -DSCALE=3 -v 0x20004
#include <metal_stdlib>
using namespace metal;
struct In { float4 p [[attribute(0)]]; half2 t [[attribute(1)]]; uchar4 c [[attribute(2)]]; };
struct Out { float4 p [[position]]; float ps [[point_size]]; float cd [[clip_distance]] [2]; half2 t [[user(texcoord)]]; float f [[flat]]; float c [[center_no_perspective]]; float s [[sample_perspective]]; bool b; };
vertex Out v(In in [[stage_in]], uint iid [[instance_id]], uint vid [[vertex_id]], constant float4x4 *mvp [[buffer(3)]])
{
    Out o; o.p = mvp[iid] * in.p * SCALE; o.ps = 4; o.cd[0] = in.p.x; o.cd[1] = in.p.y; o.t = in.t; o.f = vid; o.c = float(in.c.x); o.s = 1; o.b = vid > 3; return o;
}
struct FOut { float4 c0 [[color(0)]]; float4 c1 [[color(1), index(1)]]; float d [[depth(greater)]]; uint m [[sample_mask]]; };
fragment FOut f(Out in [[stage_in]], bool ff [[front_facing]], uint sid [[sample_id]], float4 last [[color(0)]], depth2d<float> dt [[texture(1)]], sampler s [[sampler(2)]], texture2d_array<half, access::read> ta [[texture(3)]])
{
    FOut o; o.c0 = float4(in.t.x, in.t.y, in.f, in.c) + last; o.c1 = ff ? float4(1) : float4(0); o.d = dt.sample(s, float2(0.5)) + float(ta.read(uint2(1, 2), 0).x); o.m = sid + (in.b ? 1 : 0); return o;
}

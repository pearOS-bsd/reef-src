#include <metal_stdlib>
using namespace metal;
struct VIn { float3 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; half4 color [[attribute(2)]]; };
struct VOut { float4 pos [[position]]; float2 uv; half4 color [[flat]]; float pt [[point_size]]; uint layer [[render_target_array_index]]; uint vp [[viewport_array_index]]; };
vertex VOut v(VIn in [[stage_in]], constant float4x4 &m [[buffer(1)]], uint vid [[vertex_id]], uint iid [[instance_id]], uint base [[base_vertex]], uint binst [[base_instance]], ushort amp [[amplification_id]])
{
    VOut o; o.pos = m * float4(in.pos, 1); o.uv = in.uv; o.color = in.color; o.pt = 1; o.layer = iid + base; o.vp = vid + binst + amp; return o;
}
struct FOut { half4 c [[color(0)]]; float4 c2 [[color(2)]]; };
fragment FOut f(VOut in [[stage_in]], uint prim [[primitive_id]], float2 bary [[barycentric_coord]], uint mask [[sample_mask]], bool front [[front_facing]], float2 pc [[point_coord]])
{
    FOut o; o.c = in.color + half4(prim); o.c2 = float4(bary, pc) + float4(mask) + (front ? 1 : 0); return o;
}

#include <metal_stdlib>
using namespace metal;

constant bool hasColor [[function_constant(0)]];
constant bool hasLayer [[function_constant(1)]];

struct VOut {
    float4 pos [[position]];
    float size [[point_size]];
    float clip [[clip_distance]] [2];
    float2 uv [[user(texcoord)]];
    int id [[user(ident)]];
    uint2 bits;
    float3 n;
    half4 h;
};

vertex VOut v_all(uint vid [[vertex_id]], uint iid [[instance_id]], constant float4 *p [[buffer(0)]])
{
    VOut o;
    o.pos = p[vid];
    o.size = 2.0;
    o.clip[0] = p[vid].x;
    o.clip[1] = -p[vid].y;
    o.uv = p[vid].zw;
    o.id = int(iid);
    o.bits = uint2(vid, iid);
    o.n = p[vid].xyz;
    o.h = half4(p[vid]);
    return o;
}

struct VLayer { float4 pos [[position]]; uint layer [[render_target_array_index]]; uint vp [[viewport_array_index]]; float clip [[clip_distance]]; };

vertex VLayer v_layer(uint vid [[vertex_id]], constant float4 *p [[buffer(0)]])
{
    VLayer o;
    o.pos = p[vid];
    o.layer = vid & 3;
    o.vp = vid >> 2;
    o.clip = 1.0;
    return o;
}

vertex float4 v_plain(uint vid [[vertex_id]], constant float4 *p [[buffer(3)]]) { return p[vid] * 2.0; }

struct FOut {
    float4 c0 [[color(0)]];
    int4 c1 [[color(1)]];
    uint2 c2 [[color(2)]];
    float c3 [[color(3)]];
    short4 c4 [[color(4)]];
    uchar4 c5 [[color(5)]];
    float depth [[depth(less)]];
    uint mask [[sample_mask]];
    uint stencil [[stencil]];
};

fragment FOut f_all(VOut in [[stage_in]], uint sid [[sample_id]], bool front [[front_facing]])
{
    FOut o;
    o.c0 = float4(in.uv, 0, 1);
    o.c1 = int4(in.id, -in.id, 3, -4);
    o.c2 = in.bits;
    o.c3 = in.n.x;
    o.c4 = short4(in.id);
    o.c5 = uchar4(1, 2, 3, 4);
    o.depth = front ? 0.25 : 0.75;
    o.mask = 0x5 << sid;
    o.stencil = in.bits.x & 0xff;
    return o;
}

struct Dual { float4 a [[color(0), index(0)]]; float4 b [[color(0), index(1)]]; };
fragment Dual f_dual(float4 pos [[position]]) { return {pos, pos * 0.5}; }

struct Maybe { float4 a [[color(0)]]; float4 b [[color(1), function_constant(hasColor)]]; };
fragment Maybe f_maybe(float4 pos [[position]]) { Maybe m; m.a = pos; if (hasColor) m.b = pos.yxwz; return m; }

struct VMaybe { float4 pos [[position]]; uint layer [[render_target_array_index, function_constant(hasLayer)]]; };
vertex VMaybe v_maybe(uint vid [[vertex_id]]) { VMaybe o; o.pos = float4(vid); if (hasLayer) o.layer = vid; return o; }

fragment float f_depth(float4 pos [[position]]) { return pos.z; }

struct GOut { float depth [[depth(greater)]]; };
[[early_fragment_tests]] fragment float4 f_early(float4 pos [[position]]) { return pos; }
fragment GOut f_greater(float4 pos [[position]]) { return {pos.z + 0.1}; }

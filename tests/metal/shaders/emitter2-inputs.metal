#include <metal_stdlib>
using namespace metal;

struct VIn {
    float4 position [[attribute(0)]];
    int2 icoord [[attribute(1)]];
    uint3 ucoord [[attribute(2)]];
    float scalar [[attribute(3)]];
    uchar4 colour [[attribute(4)]];
    short sh [[attribute(5)]];
    bool flag [[attribute(6)]];
    half2 h [[attribute(7)]];
};

struct VOut {
    float4 position [[position]];
    float4 colour [[user(colour)]];
    int2 icoord [[user(icoord), flat]];
    float2 c [[user(c), centroid_perspective]];
    float2 s [[user(s), sample_no_perspective]];
    float n [[user(n), center_no_perspective]];
    uint u [[user(u)]];
    float pointSize [[point_size]];
};

vertex VOut v1(VIn in [[stage_in]], uint vid [[vertex_id]], uint iid [[instance_id]], uint bv [[base_vertex]],
               uint bi [[base_instance]], constant float4x4 &m [[buffer(1)]])
{
    VOut o;
    o.position = m * in.position + float4(float(vid), float(iid), float(bv), float(bi));
    o.colour = float4(in.colour) / 255.0 + float4(in.icoord.x, in.ucoord.y, in.scalar, float(in.sh)) + (in.flag ? 1.0 : 0.0);
    o.icoord = in.icoord + int2(in.ucoord.xy);
    o.c = float2(in.h);
    o.s = in.position.zw;
    o.n = in.scalar;
    o.u = in.ucoord.z;
    o.pointSize = 2.0;
    return o;
}

fragment float4 f1(VOut in [[stage_in]], bool front [[front_facing]], float2 pc [[point_coord]], uint sid [[sample_id]],
                   uint mask [[sample_mask]], uint prim [[primitive_id]])
{
    float4 c = in.colour + float4(in.c, in.s) + float4(in.n) + float4(float(in.icoord.x), float(in.u), pc);
    if (front)
        c.x += 1.0;
    c.y += float(sid) + float(mask) + float(prim);
    return c + in.position;
}

fragment float4 f2(VOut in [[stage_in]], uint layer [[render_target_array_index]], uint vp [[viewport_array_index]])
{
    return in.colour + float4(float(layer), float(vp), 0.0, 1.0);
}

fragment float4 f3(float4 prev [[color(0)]], int4 iprev [[color(1)]], uint2 uprev [[color(2)]], half4 hprev [[color(3)]], float one [[color(4)]])
{
    return prev + float4(iprev) + float4(float2(uprev), 0.0, 0.0) + float4(hprev) + one;
}

kernel void k1(device uint *out [[buffer(0)]], uint3 gid [[thread_position_in_grid]], uint2 lid [[thread_position_in_threadgroup]],
               uint3 tg [[threadgroup_position_in_grid]], uint3 tpt [[threads_per_threadgroup]], uint3 tpg [[threads_per_grid]],
               uint3 tgs [[threadgroups_per_grid]], uint li [[thread_index_in_threadgroup]], ushort sl [[thread_index_in_simdgroup]],
               uint sw [[threads_per_simdgroup]], uint sg [[simdgroup_index_in_threadgroup]], uint ns [[simdgroups_per_threadgroup]])
{
    out[gid.x] = gid.y + gid.z + lid.x + lid.y + tg.x + tpt.y + tpg.z + tgs.x + li + sl + sw + sg + ns;
}

kernel void k2(device float *out [[buffer(0)]], threadgroup float *shared [[threadgroup(0)]], threadgroup int *other [[threadgroup(1)]],
               uint li [[thread_index_in_threadgroup]], ushort2 gid [[thread_position_in_grid]])
{
    shared[li] = float(li);
    other[li] = int(gid.y);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    out[gid.x] = shared[(li + 1) % 32] + float(other[li]);
}

kernel void k3(device float *out [[buffer(0)]], constant float *in [[buffer(1)]], device const int *ro [[buffer(2)]],
               uint id [[thread_position_in_grid]])
{
    out[id] = in[id] + float(ro[id]);
}

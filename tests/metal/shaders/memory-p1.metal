#include <metal_stdlib>
using namespace metal;
struct Out { float4 c [[color(0)]]; };
struct V { float4 p [[position]]; float2 uv; uint k [[flat]]; };
fragment Out f1(V in [[stage_in]], array<texture2d<float>, 3> tex [[texture(0)]], array<sampler, 2> s [[sampler(0)]])
{
    Out o;
    o.c = tex[in.k % 3].sample(s[in.k & 1], in.uv) + tex[(in.k + 1) % 3].read(uint2(in.uv));
    return o;
}
fragment float4 f2(V in [[stage_in]], array<texture2d<float>, 4> tex [[texture(1)]], sampler s [[sampler(0)]])
{
    return tex[in.k & 3].sample(s, in.uv, bias(1.0));
}
struct Args { texture2d<float> t [[id(0)]]; sampler s [[id(1)]]; device float* b [[id(2)]]; array<texture2d<float>, 2> ts [[id(3)]]; array<sampler, 2> ss [[id(5)]]; constant float* c [[id(7)]]; };
kernel void k1(constant Args& a [[buffer(0)]], device float4* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.t.sample(a.s, float2(0.5), level(0)) + a.ts[i & 1].sample(a.ss[i & 1], float2(0.25), level(0)) + float4(a.b[i] + a.c[i & 3]);
    a.b[i + 1] = o[i].x;
}
struct Inner { device float* p [[id(0)]]; };
struct Outer { constant Inner* inner [[id(0)]]; };
kernel void k2(constant Outer& a [[buffer(0)]], device float* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.inner->p[i];
}
struct Bufs { array<device float*, 4> b [[id(0)]]; };
kernel void k3(constant Bufs& a [[buffer(0)]], device float* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.b[1][i] + a.b[3][i * 2];
}
kernel void k4(constant Bufs& a [[buffer(0)]], device float* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.b[i & 3][i];
}
struct Mixed { int x [[id(0)]]; texture2d<float> t [[id(1)]]; float y [[id(2)]]; };
kernel void k5(constant Mixed& a [[buffer(0)]], device float4* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.t.read(uint2(i, 0)) * float(a.x) + float4(a.y);
}
kernel void k6(device Args& a [[buffer(0)]], device float4* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.t.read(uint2(i, 0)) + a.ts[1].read(uint2(i, 0));
    a.b[i] = 1.0f;
}

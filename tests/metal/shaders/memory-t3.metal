#include <metal_stdlib>
using namespace metal;
kernel void k1(array<sampler, 3> s [[sampler(0)]], texture2d<float> t [[texture(0)]], device float4* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    o[i] = t.sample(s[i % 3], float2(0.5), level(0));
}
struct Args { device float* b0 [[id(0)]]; device float* b1 [[id(1)]]; texture2d<float> t [[id(2)]]; };
kernel void k2(constant Args& a [[buffer(0)]], device float* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.b0[i] + a.b1[i * 2] + a.t.read(uint2(i, 0)).x;
}
struct Args2 { array<texture2d<float>, 3> t [[id(0)]]; device float* b [[id(3)]]; };
kernel void k3(constant Args2& a [[buffer(0)]], device float* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.t[i % 3].read(uint2(i, 0)).x + a.b[i];
}
kernel void k4(array<texture2d<float>, 4> tex [[texture(0)]], device float4* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    texture2d<float> t = tex[i & 3];
    o[i] = t.read(uint2(i, 0)) + float4(t.get_width());
}

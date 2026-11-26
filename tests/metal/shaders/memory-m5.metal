#include <metal_stdlib>
using namespace metal;
// copies (memcpy, memset), constant data, constant samplers, globals
struct Big { float4 v[6]; int k[9]; char c[3]; };
constant float table[8] = {1, 2, 3, 4, 5, 6, 7, 8};
constant half htable[5] = {1.5h, 2.5h, -3.0h, 0.25h, 65504.0h};
constant int3 itable[3] = {int3(1, 2, 3), int3(-4, 5, 6), int3(7, 8, -9)};
constant Big zero = {};
constant char bytes[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
constexpr sampler s1(coord::pixel, address::clamp_to_edge, filter::nearest);
constexpr sampler s2(address::repeat, filter::linear, mip_filter::linear, compare_func::less, max_anisotropy(8), lod_clamp(1.0f, 7.5f));
constexpr sampler s3(s_address::mirrored_repeat, t_address::clamp_to_border, r_address::clamp_to_zero, border_color::opaque_white, min_filter::linear, mag_filter::nearest);
kernel void k1(device Big* o [[buffer(0)]], device const Big* in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    Big b = in[i];
    b.k[i % 9] = int(i);
    o[i] = b;
    o[i + 1] = zero;
}
kernel void k2(device float* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    o[i] = table[i & 7] + float(htable[i % 5]) + float(itable[i % 3].y) + float(bytes[i % 11]);
}
kernel void k3(texture2d<float> t [[texture(0)]], device float4* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    o[i] = t.sample(s1, float2(i, 0)) + t.sample(s2, float2(0.5), level(1)) + t.sample(s3, float2(0.25), level(0));
}
kernel void k4(device char* o [[buffer(0)]], device const char* in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    char local[23];
    for (int j = 0; j < 23; j++) local[j] = in[j * i];
    for (int j = 0; j < 23; j++) o[j + i] = local[22 - j];
}
kernel void k5(device Big* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    Big b;
    for (int j = 0; j < 6; j++) b.v[j] = float4(0);
    for (int j = 0; j < 9; j++) b.k[j] = 0;
    b.c[0] = b.c[1] = b.c[2] = 0;
    o[i] = b;
}

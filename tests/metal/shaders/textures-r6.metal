#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
uint2 plain2u2(H, S, float2) __asm__("air.sample_texture_2d.u.v2i32");
fragment float4 r6(texture2d<uint> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(float2(plain2u2(t.__h, s.__handle(), p.xy)), 0, 0); }

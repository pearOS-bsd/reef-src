#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
short4 plain2s(H, S, float2) __asm__("air.sample_texture_2d.s.v4i16");
fragment float4 r7(texture2d<short> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2s(t.__h, s.__handle(), p.xy)); }

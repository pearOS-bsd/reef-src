#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
ushort plain2us(H, S, float2) __asm__("air.sample_texture_2d.u.i16");
fragment float4 r8(texture2d<ushort> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(float(plain2us(t.__h, s.__handle(), p.xy))); }

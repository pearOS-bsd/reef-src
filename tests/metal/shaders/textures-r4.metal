#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float plain2f(H, S, float2) __asm__("air.sample_texture_2d.f32");
fragment float4 r4(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2f(t.__h, s.__handle(), p.xy)); }

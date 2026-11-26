#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 plain2(H, S, float2) __asm__("air.sample_texture_2d.v4f32");
float2 plain2v2(H, S, float2) __asm__("air.sample_texture_2d.v2f32");
float4 few(H) __asm__("air.sample_texture_2d.v4f32");
fragment float4 r1(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return plain2(t.__h, s.__handle(), p.xy); }
fragment float4 r31(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return few(t.__h); }

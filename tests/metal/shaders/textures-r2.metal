#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float2 plain2v2(H, S, float2) __asm__("air.sample_texture_2d.v2f32");
fragment float4 r2(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2v2(t.__h, s.__handle(), p.xy), 0, 1); }

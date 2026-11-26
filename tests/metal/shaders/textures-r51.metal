#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 nosampler(H, H, float2) __asm__("air.sample_texture_2d.v4f32");
fragment float4 r51(texture2d<float> t [[texture(0)]], texture2d<float> u [[texture(1)]], float4 p [[position]]) { return nosampler(t.__h, u.__h, p.xy); }

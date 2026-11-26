#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 offset2s(H, S, float2, bool, int) __asm__("air.sample_texture_2d.v4f32");
fragment float4 r14(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2s(t.__h, s.__handle(), p.xy, true, 5); }

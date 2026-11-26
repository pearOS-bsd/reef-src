#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 offset2(H, S, float2, bool, int2) __asm__("air.sample_texture_2d.v4f32");
fragment float4 r13(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2(t.__h, s.__handle(), p.xy, true, int2(3, -4)); }

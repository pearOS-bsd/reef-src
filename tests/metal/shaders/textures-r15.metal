#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 offset2l(H, S, float2, bool, long2) __asm__("air.sample_texture_2d.v4f32");
fragment float4 r15(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2l(t.__h, s.__handle(), p.xy, true, long2(5, 6)); }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 gather2n(H, S, float2) __asm__("air.gather_texture_2d.v4f32");
fragment float4 r17(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return gather2n(t.__h, s.__handle(), p.xy); }

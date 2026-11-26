#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 gather2(H, S, float2, bool, int2, int) __asm__("air.gather_texture_2d.v4f32");
fragment float4 r16(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return gather2(t.__h, s.__handle(), p.xy, true, int2(0), int(p.x)); }

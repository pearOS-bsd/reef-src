#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
int4 plain2i(H, S, float2) __asm__("air.sample_texture_2d.s.v4i32");
fragment float4 r5(texture2d<int> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2i(t.__h, s.__handle(), p.xy)); }

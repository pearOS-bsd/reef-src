#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
S readSampler() __asm__("air.get_read_sampler");
float4 readS2(H, S, uint2) __asm__("air.read_texture_2d.v4f32");
fragment float4 r41(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return readS2(t.__h, readSampler(), uint2(p.xy)); }

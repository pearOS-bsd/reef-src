#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
S readSampler() __asm__("air.get_read_sampler");
float4 readS(H, S, uint2, uint2, uint) __asm__("air.read_texture_2d.v4f32");
fragment float4 r38(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return readS(t.__h, readSampler(), uint2(p.xy), uint2(1, 2), 0); }

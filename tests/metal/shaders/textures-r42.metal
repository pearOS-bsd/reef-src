#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
S readSampler() __asm__("air.get_read_sampler");
float4 readArr(H, S, uint2, uint2, uint, uint) __asm__("air.read_texture_2d_array.v4f32");
fragment float4 r42(texture2d_array<float> t [[texture(0)]], float4 p [[position]]) { return readArr(t.__h, readSampler(), uint2(p.xy), uint2(p.yx), 3, 1); }

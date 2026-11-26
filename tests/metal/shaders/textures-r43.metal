#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 read2(H, uint2) __asm__("air.read_texture_2d.v4f32");
fragment float4 r43(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return read2(t.__h, uint2(p.xy)); }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
uint query2(H, uint) __asm__("air.get_depth_texture_2d");
fragment float4 r34(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query2(t.__h, 1)); }

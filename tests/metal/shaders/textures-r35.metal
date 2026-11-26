#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
uint query3(H) __asm__("air.get_num_samples_texture_2d");
fragment float4 r35(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query3(t.__h)); }

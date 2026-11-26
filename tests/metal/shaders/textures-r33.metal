#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
uint query(H) __asm__("air.get_foo_texture_2d");
fragment float4 r33(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query(t.__h)); }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
uint4 query5(H) __asm__("air.get_width_texture_2d");
fragment float4 r37(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query5(t.__h)); }

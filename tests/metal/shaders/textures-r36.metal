#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
uint query4(H) __asm__("air.get_width_something");
fragment float4 r36(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query4(t.__h)); }

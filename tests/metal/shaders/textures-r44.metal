#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
void write2(H, uint2, float2, uint) __asm__("air.write_texture_2d.v4f32");
fragment float4 r44(texture2d<float, access::write> t [[texture(0)]], float4 p [[position]]) { write2(t.__h, uint2(p.xy), p.xy, 0); return 0; }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
void writef(H, uint2, float) __asm__("air.write_texture_2d.v4f32");
fragment float4 r46(texture2d<float, access::write> t [[texture(0)]], float4 p [[position]]) { writef(t.__h, uint2(p.xy), p.x); return 0; }

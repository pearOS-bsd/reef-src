#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
void writeu(H, uint2, uint4) __asm__("air.write_texture_2d.u.v4i32");
fragment float4 r48(texture2d<uint, access::write> t [[texture(0)]], float4 p [[position]]) { writeu(t.__h, uint2(p.xy), uint4(p)); return 0; }

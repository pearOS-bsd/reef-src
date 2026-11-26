#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
void writes(H, uint2, short4) __asm__("air.write_texture_2d.s.v4i16");
fragment float4 r49(texture2d<short, access::write> t [[texture(0)]], float4 p [[position]]) { writes(t.__h, uint2(p.xy), short4(p)); return 0; }

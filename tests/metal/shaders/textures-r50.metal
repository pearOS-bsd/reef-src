#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
void writeus(H, uint2, ushort4) __asm__("air.write_texture_2d.u.v4i16");
fragment float4 r50(texture2d<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { writeus(t.__h, uint2(p.xy), ushort4(p)); return 0; }

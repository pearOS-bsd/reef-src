#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
void writei(H, uint2, int4) __asm__("air.write_texture_2d.s.v4i32");
fragment float4 r47(texture2d<int, access::write> t [[texture(0)]], float4 p [[position]]) { writei(t.__h, uint2(p.xy), int4(p)); return 0; }

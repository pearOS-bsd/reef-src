#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
fragment float4 r57(texture2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { t.write(float4(1), uint2(p.xy)); return 0; }

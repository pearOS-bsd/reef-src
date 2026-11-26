#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
fragment float4 r54(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], sampler z [[sampler(1)]], float4 p [[position]]) { return t.sample(s, p.xy) + t.sample(z, p.yx) + t.sample(s, p.zw) + t.read(uint2(p.xy)) + float4(t.get_width()); }

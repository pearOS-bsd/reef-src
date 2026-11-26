#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
fragment float4 r55(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy) + t.sample_compare(s, p.xy, p.z) + t.sample(s, p.yx) + t.sample_compare(s, p.yx, p.w)); }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
fragment float4 r61(depth2d_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, 1, p.w, level(2.0)) + t.sample_compare(s, p.xy, 1, p.w)); }

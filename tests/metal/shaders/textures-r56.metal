#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
vertex float4 r56(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], uint v [[vertex_id]]) { float4 p = float4(float(v)); return float4(t.sample_compare(s, p.xyz, 2, p.w)) + float4(t.sample(s, p.xyz, 1)); }

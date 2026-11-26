#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
vertex float4 r62(depth2d_array<float> t [[texture(0)]], depthcube<float> c [[texture(1)]], sampler s [[sampler(0)]], uint v [[vertex_id]]) { float4 p = float4(float(v)); return float4(t.sample_compare(s, p.xy, 1, p.w) + c.sample_compare(s, p.xyz, p.w) + c.sample_compare(s, p.xyz, p.w, level(1.0))); }

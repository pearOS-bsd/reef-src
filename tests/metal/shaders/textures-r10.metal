#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float2 compare2(H, S, float2, float) __asm__("air.sample_compare_depth_2d.v2f32");
fragment float4 r10(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compare2(t.__h, s.__handle(), p.xy, p.z), 0, 0); }

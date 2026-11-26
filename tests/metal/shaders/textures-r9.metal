#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 compare4(H, S, float2, float) __asm__("air.sample_compare_depth_2d.v4f32");
fragment float4 r9(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return compare4(t.__h, s.__handle(), p.xy, p.z); }

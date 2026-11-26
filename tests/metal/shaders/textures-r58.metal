#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 compareCubeArray(H, S, float3, uint, float) __asm__("air.sample_compare_depth_cube_array.v4f32");
fragment float4 r58(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return compareCubeArray(t.__h, s.__handle(), p.xyz, 2, p.w); }

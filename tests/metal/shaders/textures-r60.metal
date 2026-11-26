#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float compareCubeArrayLevel(H, S, float3, uint, float, bool, float, float, int) __asm__("air.sample_compare_depth_cube_array.f32");
fragment float4 r60(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compareCubeArrayLevel(t.__h, s.__handle(), p.xyz, 2, p.w, true, 1.0, 0.5, 0)); }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float cubeArrayGrad(H, S, float3, uint, float, float3, float3) __asm__("air.sample_compare_depth_cube_array_grad.f32");
fragment float4 r52(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(cubeArrayGrad(t.__h, s.__handle(), p.xyz, 2, p.w, p.xyz, p.zyx)); }

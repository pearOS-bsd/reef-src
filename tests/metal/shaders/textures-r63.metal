#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 cube(H, S, float3) __asm__("air.sample_texture_cube.v4f32");
float4 one(H, S, float) __asm__("air.sample_texture_1d.v4f32");
float4 three(H, S, float3) __asm__("air.sample_texture_3d.v4f32");
kernel void r63(texturecube<float> t [[texture(0)]], texture1d<float> o [[texture(1)]], texture3d<float> d [[texture(2)]], sampler s [[sampler(0)]], device float4 *b [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = b[id]; b[id] = cube(t.__h, s.__handle(), p.xyz) + one(o.__h, s.__handle(), p.x) + three(d.__h, s.__handle(), p.xyz); }

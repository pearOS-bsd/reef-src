#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 grad2b(H, S, float2, float2, float2, bool, int2) __asm__("air.sample_texture_2d_grad.v4f32");
fragment float4 r30(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return grad2b(t.__h, s.__handle(), p.xy, p.xy, p.yx, true, int2(1, 2)); }

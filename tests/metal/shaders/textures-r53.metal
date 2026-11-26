#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 onBuffer(device float *, S, float2) __asm__("air.sample_texture_2d.v4f32");
fragment float4 r53(device float *b [[buffer(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return onBuffer(b, s.__handle(), p.xy); }

#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float compare1(H, S, float, float) __asm__("air.sample_compare_depth_2d.f32");
fragment float4 r11(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compare1(t.__h, s.__handle(), p.x, p.z)); }

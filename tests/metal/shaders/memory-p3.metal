#include <metal_stdlib>
using namespace metal;
struct Args { texture2d<float> t [[id(0)]]; sampler s [[id(1)]]; device float* b [[id(2)]]; device float* c [[id(3)]]; };
kernel void k(constant Args& a [[buffer(0)]], device float4* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    o[i] = a.t.sample(a.s, float2(0.5), level(0)) + float4(a.b[i] + a.c[i]);
}

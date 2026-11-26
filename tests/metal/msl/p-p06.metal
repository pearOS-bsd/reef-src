#include <metal_stdlib>
using namespace metal;
constant float a = sin(0.5f);
kernel void k(device float *o [[buffer(0)]], device const float *in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    if (in[i] > 0) o[i] = a; else o[i] = a * 2;
    o[i + 1] = a + in[0];
}
fragment float4 f(float4 p [[position]]) { return float4(a); }

#include <metal_stdlib>
using namespace metal;
kernel void k1(device uint4 *o [[buffer(0)]], device float4 *f [[buffer(1)]], threadgroup uint *t [[threadgroup(0)]], uint i [[thread_position_in_grid]])
{
    o[i] = uint4(i);
    f[i] = float4(i);
    t[i] = i;
}
fragment float4 f1(float4 p [[position]], device float4 *b [[buffer(0)]])
{
    return p + b[0];
}

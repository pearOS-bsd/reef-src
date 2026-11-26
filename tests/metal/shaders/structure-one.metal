#include <metal_stdlib>
using namespace metal;
kernel void one(device uint *o [[buffer(0)]], constant uint &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    uint s = 0;
    for (uint i = 0; i < n; ++i)
        s += o[i];
    o[id] = s;
}

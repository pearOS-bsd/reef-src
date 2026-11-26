#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]], uint l [[thread_index_in_threadgroup]])
{
    if (l == 0) threadgroup float bad[4];
    o[i] = 0;
}

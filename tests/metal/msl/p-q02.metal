#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    float local[4] = {1, 2, 3, 4};
    thread float *p = i > 3 ? &local[0] : &local[1];
    o[i] = p[i & 1];
}

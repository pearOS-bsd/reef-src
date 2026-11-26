#include <metal_stdlib>
using namespace metal;
float pick(thread float *p, uint i) { return p[i]; }
kernel void k(device float *o [[buffer(0)]], device float *in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    float local[4] = {1, 2, 3, float(i)};
    thread float *p = local;
    for (uint j = 0; j < i; j++) p = p + 1;
    o[i] = pick(p, 0);
}

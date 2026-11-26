#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], threadgroup float *t [[threadgroup(0)]], uint i [[thread_position_in_grid]])
{
    float local = 2;
    thread float *p = &local;
    threadgroup float *q = t;
    o[i] = *p + q[i];
    thread float *r = (thread float *)o;
    o[i + 1] = *r;
}

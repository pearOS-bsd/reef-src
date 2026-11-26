#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
#if 1
    threadgroup float x[4];
#endif
    x[0] = 1;
#define LOCAL_TG threadgroup float y[4];
    LOCAL_TG
    y[0] = 2;
    o[i] = x[0] + y[0];
}

#include <metal_stdlib>
using namespace metal;
#warning "a warning from the program"
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    int unused = 1.5;
    o[i] = unused;
}

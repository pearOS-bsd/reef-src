#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    auto f = [](float x) device { return x * 2; };
    o[i] = f(o[i]);
}

#include <metal_stdlib>
using namespace metal;
constexpr float k1 = sin(1.0f);
constexpr constant float k2 = cos(0.5f) * 2;
constant constexpr float k3 = k1 + k2;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    constexpr float local = sqrt(2.0f);
    o[i] = k1 + k2 + k3 + local;
}

#include <metal_stdlib>
using namespace metal;
kernel void k(device uint* a [[buffer(0)]], device uint* b [[buffer(1)]], constant uint& n [[buffer(2)]], threadgroup uint* t [[threadgroup(0)]], uint i [[thread_position_in_grid]])
{
    a[i] = b[i] + n + t[i];
}

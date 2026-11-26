#include <metal_stdlib>
using namespace metal;
kernel void k(device uchar *o [[buffer(0)]], device const uchar *in [[buffer(1)]], uint i [[thread_position_in_grid]], uint n [[threads_per_grid]])
{
    uchar buffer[64];
    for (uint j = 0; j < 64; j++) buffer[j] = in[(i + j) % n];
    uchar copy[64];
    for (uint j = 0; j < 64; j++) copy[j] = buffer[63 - j];
    for (uint j = 0; j < 64; j++) o[i * 64 + j] = copy[j];
    uchar zero[100] = {};
    o[i] += zero[i % 100];
}

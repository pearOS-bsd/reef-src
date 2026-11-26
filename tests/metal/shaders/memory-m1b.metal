#include <metal_stdlib>
using namespace metal;
kernel void kbf(device bfloat* a [[buffer(0)]], device const bfloat* b [[buffer(1)]], uint i [[thread_position_in_grid]]) { a[i] = b[i] * bfloat(2.0f); }
kernel void kbf4(device bfloat4* a [[buffer(0)]], device const bfloat4* b [[buffer(1)]], uint i [[thread_position_in_grid]]) { a[i] = b[i] + b[i + 1]; }

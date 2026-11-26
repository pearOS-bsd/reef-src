#include <metal_stdlib>
using namespace metal;
extern float scale(float x);
extern float shifted(float x);
kernel void k(device float *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = scale(a[i]) + shifted(a[i]); }
kernel void plain(device float *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = a[i] + 1.0f; }
extern int missing(int x);
kernel void m(device int *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = missing(a[i]); }
extern int twice(float x, float y);
kernel void wrong(device int *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = twice(1.0f, 2.0f); }

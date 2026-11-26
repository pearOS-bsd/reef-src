#include <metal_stdlib>
using namespace metal;
[[visible]] float scale(float x);
[[visible]] float shifted(float x);
kernel void k(device float *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = scale(a[i]) + shifted(a[i]); }
[[visible]] int missing(int x);
kernel void m(device int *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = missing(a[i]); }
[[visible]] int twice(float x, float y);
kernel void wrong(device int *a [[buffer(0)]], uint i [[thread_position_in_grid]]) { a[i] = twice(1.0f, 2.0f); }
[[visible]] float twice2(float x) { return scale(x) * 2.0f; }

#include <metal_stdlib>
using namespace metal;
float slow(int n) { float s = 0; for (int i = 0; i < n; i++) s += sin(float(i)); return s; }
constant float total = slow(1000);
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = total; }

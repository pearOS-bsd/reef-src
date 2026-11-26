#include <metal_stdlib>
using namespace metal;
struct Holder { int values[4]; Holder() { for (int i = 0; i < 4; i++) values[i] = i * i + int(sin(float(i)) * 10); } };
constant Holder h;
kernel void k(device int *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = h.values[i & 3]; }

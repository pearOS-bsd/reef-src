#include <metal_stdlib>
using namespace metal;
constant half hv = half(sin(0.75f));
constant int iv = int(floor(exp(2.0f)));
constant uint uv = uint(iv) * 3u;
constant bool bv = hv > 0.5h;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = float(hv) + float(iv) + float(uv) + (bv ? 1.0f : 0.0f); }

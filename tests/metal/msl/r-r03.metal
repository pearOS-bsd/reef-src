#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]], uint l [[thread_index_in_threadgroup]])
{
    threadgroup float a[8];
    if (l == 0) { threadgroup float b[8]; b[0] = 1; a[0] = b[0]; }
    for (int j = 0; j < 2; j++) { threadgroup int c; c = j; a[1] = c; }
    switch (l) { case 0: { threadgroup float d; d = 2; a[2] = d; } break; default: break; }
    struct Local { float x; float y; };
    Local q = {1, 2};
    threadgroup float e[2];
    e[0] = q.x;
    o[i] = a[l & 7] + e[0];
}

#include <metal_stdlib>
using namespace metal;
struct Big { float4 a, b, c, d; };
kernel void k(device Big *o [[buffer(0)]], device const Big *in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    Big local[4];
    for (int j = 0; j < 4; j++) local[j] = in[i + j];
    o[i] = local[i & 3];
    thread float *p = &local[0].a.x;
    p[i & 15] = 1;
    o[i + 1] = local[(i + 1) & 3];
}

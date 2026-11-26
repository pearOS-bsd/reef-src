#include <metal_stdlib>
using namespace metal;
struct Big { float4 v[8]; int tail[13]; };
kernel void k(device Big *o [[buffer(0)]], device const Big *in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    Big b = in[i];
    Big c = b;
    c.tail[i % 13] = int(i);
    o[i] = c;
    Big z = {};
    o[i + 1] = z;
}

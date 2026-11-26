#include <metal_stdlib>
using namespace metal;
#define SHARED [[threadgroup(0)]]
#define STR(x) #x
#define PTR(a) device float *a
kernel void k(PTR(o) [[buffer(0)]], threadgroup float *t SHARED, uint i [[thread_position_in_grid]])
{
    t[i & 7] = o[i];
    o[i] = t[0] + sizeof(STR(abc));
}

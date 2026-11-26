//opt -v 0x20002
#include <metal_stdlib>
using namespace metal;
#ifndef GAIN
#define GAIN 1
#endif
struct S { float v; float get() device const { return v * 2; } float get() thread const { return v; } };
kernel void k(device float *o [[buffer(0)]], device const S *s [[buffer(1)]], constant S &c [[buffer(2)]], threadgroup S *t [[threadgroup(0)]], uint i [[thread_position_in_grid]], uint l [[thread_index_in_threadgroup]])
{
    threadgroup float local[16];
    { threadgroup int counter; if (l == 0) counter = 0; }
    local[l & 15] = s[i].get() + c.v + t[0].v;
    threadgroup_barrier(mem_flags::mem_threadgroup);
    S here = {local[(l + 1) & 15]};
    float x = here.get() * GAIN;
#if __METAL_VERSION__ >= 300
    x += 3;
#elif __METAL_VERSION__ >= 200
    x += 2;
#else
    x += 1;
#endif
#ifdef __HAVE_NATIVE_FAST_MATH__
    x += 0.5;
#endif
#ifdef EXTRA
    x += EXTRA;
#endif
    o[i] = sin(x) / x + sqrt(x) + fast::cos(x) + precise::tan(x) + (isnan(x) ? 1 : 0) + fma(x, x, x) + copysign(x, -1.0f) + exp10(x);
}

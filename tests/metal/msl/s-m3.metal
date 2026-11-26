#include <metal_stdlib>
using namespace metal;
struct P { atomic_int count; atomic_uint flags; };
kernel void k(device P &p [[buffer(0)]], device atomic_float *f [[buffer(1)]], texture2d<float, access::write> out [[texture(0)]], texture2d<float, access::read_write> rw [[texture(1)]], threadgroup int *tg [[threadgroup(0)]], uint2 gid [[thread_position_in_grid]], uint w [[threads_per_simdgroup]])
{
    int old = atomic_fetch_add_explicit(&p.count, 1, memory_order_relaxed);
    atomic_fetch_or_explicit(&p.flags, 2u, memory_order_relaxed);
    atomic_store_explicit(f, float(old), memory_order_relaxed);
    tg[gid.x & 7] = old;
    simdgroup_barrier(mem_flags::mem_none);
    float4 c = rw.read(gid);
    rw.write(c * 2, gid);
    out.write(float4(float(tg[0]), float(w), out.get_width(), out.get_height()), gid);
}

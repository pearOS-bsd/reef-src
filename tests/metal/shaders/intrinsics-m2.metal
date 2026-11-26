#include <metal_stdlib>
using namespace metal;
kernel void k1(device atomic_int *a [[buffer(0)]], device atomic_uint *b [[buffer(1)]], device atomic_float *c [[buffer(2)]],
               device int *o [[buffer(3)]], uint i [[thread_position_in_grid]], uint l [[thread_index_in_threadgroup]])
{
    threadgroup atomic_int ta[16];
    threadgroup atomic_uint tb[16];
    int x = atomic_fetch_add_explicit(&a[i], 1, memory_order_relaxed);
    x += atomic_fetch_sub_explicit(&a[i], 2, memory_order_relaxed);
    x += atomic_fetch_min_explicit(&a[i], 3, memory_order_relaxed);
    x += atomic_fetch_max_explicit(&a[i], 4, memory_order_relaxed);
    x += atomic_fetch_and_explicit(&a[i], 5, memory_order_relaxed);
    x += atomic_fetch_or_explicit(&a[i], 6, memory_order_relaxed);
    x += atomic_fetch_xor_explicit(&a[i], 7, memory_order_relaxed);
    x += atomic_exchange_explicit(&a[i], 8, memory_order_relaxed);
    x += atomic_load_explicit(&a[i], memory_order_relaxed);
    atomic_store_explicit(&a[i], x, memory_order_relaxed);

    bool ok = true; int e = 3;
    uint y = atomic_fetch_add_explicit(&b[i], 1u, memory_order_relaxed);
    y += atomic_fetch_min_explicit(&b[i], 3u, memory_order_relaxed);
    y += atomic_fetch_max_explicit(&b[i], 4u, memory_order_relaxed);
    y += atomic_load_explicit(&b[i], memory_order_relaxed);
    float z = atomic_fetch_add_explicit(&c[i], 1.5f, memory_order_relaxed);
    z += atomic_load_explicit(&c[i], memory_order_relaxed);
    atomic_store_explicit(&c[i], z, memory_order_relaxed);
    z += atomic_exchange_explicit(&c[i], 2.5f, memory_order_relaxed);
    atomic_store_explicit(&ta[l & 15], int(y), memory_order_relaxed);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    int w = atomic_fetch_add_explicit(&ta[(l + 1) & 15], 1, memory_order_relaxed);
    uint v = atomic_fetch_or_explicit(&tb[l & 15], 2u, memory_order_relaxed);
    threadgroup_barrier(mem_flags::mem_device);
    threadgroup_barrier(mem_flags::mem_device | mem_flags::mem_threadgroup | mem_flags::mem_texture);
    threadgroup_barrier(mem_flags::mem_none);
    simdgroup_barrier(mem_flags::mem_threadgroup);
    o[i] = x + int(y) + int(z) + w + int(v) + int(ok) + e;
}
fragment float4 f1(float4 p [[position]], device float4 *b [[buffer(0)]])
{
    float4 d = dfdx(p) + dfdy(p.yxzw) + fwidth(p);
    float2 e = dfdx(p.xy) + fwidth(p.zw);
    if (p.x > 10.0)
        discard_fragment();
    return d + float4(e, 0.0, float(get_num_samples()));
}
kernel void k2(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    o[i] = dfdx(o[i]);
}
fragment float4 f2(float4 p [[position]])
{
    threadgroup_barrier(mem_flags::mem_device);
    return p;
}
kernel void k3(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    simdgroup_barrier(mem_flags::mem_none);
    o[i] = 1.0;
}

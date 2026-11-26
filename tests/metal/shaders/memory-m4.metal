#include <metal_stdlib>
using namespace metal;
// atomics
kernel void k1(device atomic_int* a [[buffer(0)]], device atomic_uint* u [[buffer(1)]], device int* o [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    int x = atomic_fetch_add_explicit(&a[0], 1, memory_order_relaxed);
    x += atomic_fetch_sub_explicit(&a[1], 2, memory_order_relaxed);
    x += atomic_fetch_and_explicit(&a[2], 3, memory_order_relaxed);
    x += atomic_fetch_or_explicit(&a[3], 4, memory_order_relaxed);
    x += atomic_fetch_xor_explicit(&a[4], 5, memory_order_relaxed);
    x += atomic_fetch_min_explicit(&a[5], 6, memory_order_relaxed);
    x += atomic_fetch_max_explicit(&a[6], 7, memory_order_relaxed);
    x += atomic_exchange_explicit(&a[7], 8, memory_order_relaxed);
    x += atomic_load_explicit(&a[8], memory_order_relaxed);
    atomic_store_explicit(&a[9], x, memory_order_relaxed);
    int e = 3;
    uint y = atomic_fetch_min_explicit(&u[0], 6u, memory_order_relaxed) + atomic_fetch_max_explicit(&u[1], 7u, memory_order_relaxed);
    o[i] = x + e + int(y);
}
kernel void k2(threadgroup atomic_int* a [[threadgroup(0)]], device int* o [[buffer(0)]], uint i [[thread_position_in_grid]], uint l [[thread_position_in_threadgroup]])
{
    threadgroup atomic_uint counter;
    if (l == 0) atomic_store_explicit(&counter, 0u, memory_order_relaxed);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    uint c = atomic_fetch_add_explicit(&counter, 1u, memory_order_relaxed);
    int m = atomic_fetch_min_explicit(&a[l & 3], int(i), memory_order_relaxed);
    o[i] = int(c) + m + atomic_fetch_max_explicit(&a[4], -int(i), memory_order_relaxed);
}
kernel void k3(device atomic_float* f [[buffer(0)]], device float* o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    float a = atomic_fetch_add_explicit(&f[0], 1.5f, memory_order_relaxed);
    a += atomic_exchange_explicit(&f[1], 2.0f, memory_order_relaxed);
    a += atomic_load_explicit(&f[2], memory_order_relaxed);
    atomic_store_explicit(&f[3], a, memory_order_relaxed);
    o[i] = a;
}

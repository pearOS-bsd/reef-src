#include <metal_stdlib>
using namespace metal;

kernel void ints(device int *out [[buffer(0)]], device uint *u [[buffer(1)]], device short *s [[buffer(2)]], device uchar *c [[buffer(3)]],
                 device long *l [[buffer(4)]], device ulong *ul [[buffer(5)]], uint id [[thread_position_in_grid]])
{
    int a = out[id], b = out[id + 1];
    out[id] = a / b + a % b + (a >> 3) + (a << 2) - (a ^ b) + (a | b) + (a & b) * b;
    u[id] = u[id] / u[id + 1] + u[id] % 7u + (u[id] >> 5) + (u[id] << 1) + ~u[id];
    s[id] = short(s[id] * s[id + 1] + s[id] / short(3) - short(a));
    c[id] = uchar(c[id] + c[id + 1] * uchar(3) - (c[id] >> 1));
    l[id] = l[id] * l[id + 1] + (l[id] >> 7) + l[id] / 3 + long(a);
    ul[id] = ul[id] % 9ul + (ul[id] << 40) + ulong(u[id]);
    out[id + 2] = int(s[id]) + int(char(c[id])) + int(l[id] >> 32) + int(ul[id]);
    out[id + 3] = (a < b) + (a <= b) * 2 + (a > b) * 4 + (a >= b) * 8 + (a == b) * 16 + (a != b) * 32 + (u[id] < u[id + 1]) * 64;
    out[id + 4] = abs(a) + min(a, b) + max(a, b) + clamp(a, 0, 10) + popcount(u[id]) + clz(u[id]) + ctz(u[id]);
}

kernel void floats(device float *f [[buffer(0)]], device half *h [[buffer(1)]], device int *i [[buffer(2)]], device uint *u [[buffer(3)]],
                   device bool *b [[buffer(4)]], uint id [[thread_position_in_grid]])
{
    float x = f[id], y = f[id + 1];
    f[id] = fmod(x, y) + -x + x * y - x / y;
    h[id] = half(x) + h[id] * half(2.5) + half(i[id]) + half(u[id]);
    i[id] = int(x) + int(h[id]) + int(y < x) + int(isnan(x)) + int(isinf(y));
    u[id] = uint(x) + uint(h[id + 1]) + uint(x == y) + uint(x != y);
    b[id] = x < y || x >= y || bool(i[id]) || bool(x);
    f[id + 1] = float(b[id]) + float(char(i[id])) + float(ushort(u[id])) + float(long(i[id])) + float(ulong(u[id]));
    f[id + 2] = select(x, y, x > y) + (x <= y ? 1.0 : 2.0) + float(!(x < y)) + float(!(x >= y)) + float(!(x == y));
    f[id + 3] = as_type<float>(i[id]) + as_type<float>(u[id] + 1u) + float(as_type<int>(x)) + float(as_type<half2>(u[id]).x);
}

kernel void vectors(device float4 *f [[buffer(0)]], device int4 *i [[buffer(1)]], device bool4 *b [[buffer(2)]], device uint2 *u [[buffer(3)]],
                    device half3 *h [[buffer(4)]], uint id [[thread_position_in_grid]])
{
    float4 x = f[id], y = f[id + 1];
    f[id] = x.wzyx + y.xxyy + float4(x.xy, y.zw) + x.zzzz + float4(x.x, 1.0, y.y, 2.0);
    bool4 c = x < y;
    b[id] = bool4(c.x && x.x != y.x, c.y, !c.z, c.w || x.w == y.w);
    i[id] = select(i[id], i[id + 1], c) + int4(c.x, c.y, c.z, c.w) + i[id].yxwz * int4(2, 3, 4, 5);
    u[id] = u[id].yx + uint2(any(c), all(c)) + uint2(any(x > 0.0), all(i[id] == int4(1)));
    h[id] = half3(x.xyz) + h[id].zyx + half3(y.xyz);
    float3 t = x.xyz;
    t[id & 3] = 7.0;
    f[id + 2] = float4(t, x[id & 1]) + float4(f[id].w);
    f[id + 3] = float4(1.0) + float4(0.0, 1.0, 2.0, 3.0) + float4(INFINITY, -INFINITY, NAN, -0.0);
}

struct Pair { float a; int b[3]; float2 c; };

kernel void aggregates(device Pair *p [[buffer(0)]], device float *out [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    Pair q = p[id];
    q.b[id % 3] = int(q.a);
    q.c.y = q.a;
    p[id + 1] = q;
    out[id] = q.c.x + float(q.b[1]);
}

kernel void atomics(device atomic_int *a [[buffer(0)]], device atomic_uint *u [[buffer(1)]], device atomic_float *f [[buffer(2)]],
                    device int *out [[buffer(3)]], uint id [[thread_position_in_grid]])
{
    int old = atomic_fetch_add_explicit(&a[0], 1, memory_order_relaxed);
    old += atomic_fetch_sub_explicit(&a[1], 2, memory_order_relaxed);
    old += atomic_fetch_max_explicit(&a[2], int(id), memory_order_relaxed);
    old += atomic_fetch_min_explicit(&a[3], int(id), memory_order_relaxed);
    old += int(atomic_fetch_max_explicit(&u[0], id, memory_order_relaxed));
    old += int(atomic_fetch_min_explicit(&u[1], id, memory_order_relaxed));
    old += int(atomic_fetch_and_explicit(&u[2], id, memory_order_relaxed));
    old += int(atomic_fetch_or_explicit(&u[3], id, memory_order_relaxed));
    old += int(atomic_fetch_xor_explicit(&u[4], id, memory_order_relaxed));
    old += int(atomic_exchange_explicit(&u[5], id, memory_order_relaxed));
    int expected = 3;
    bool done = id > 3;
    old += int(atomic_fetch_add_explicit(&f[0], 1.5, memory_order_relaxed));
    atomic_store_explicit(&a[5], old, memory_order_relaxed);
    out[id] = old + int(done) + expected + atomic_load_explicit(&a[6], memory_order_relaxed);
}

kernel void loops(device float *out [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    float tmp[8];
    for (int i = 0; i < 8; ++i)
        tmp[i] = out[id + i] * float(i);
    float sum = 0.0;
    for (int i = 0; i < 8; ++i)
        sum += tmp[(i * 3) & 7];
    float big[64];
    for (int i = 0; i < 64; ++i)
        big[i] = float(i);
    for (int i = 0; i < 64; ++i)
        sum += big[(i + int(id)) & 63];
    out[id] = sum;
}

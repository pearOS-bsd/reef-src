#include <metal_stdlib>
using namespace metal;
// structs, arrays, matrices, unaligned and packed members
struct A { char c; float f; short s; uchar u; packed_float3 p; half h; long l; };
struct B { float3x3 m; float2x4 n; half3x2 hm; A a[3]; int x[5]; };
struct __attribute__((packed)) P { char c; int i; short s; float f; };
kernel void k1(device B* b [[buffer(0)]], device const B* c [[buffer(1)]], uint i [[thread_position_in_grid]]) { b[i] = c[i + 1]; }
kernel void k2(device B* b [[buffer(0)]], device const B* c [[buffer(1)]], uint i [[thread_position_in_grid]]) { b[i].m = c[i].m * c[i].m; b[i].a[i & 1].f = c[i].a[2].p.y; b[i].x[i % 5] = c[i].a[1].l; b[i].hm = c[i].hm; }
kernel void k3(device P* p [[buffer(0)]], device const P* q [[buffer(1)]], uint i [[thread_position_in_grid]]) { p[i].i = q[i].i + 1; p[i].f = q[i].f; p[i].s = q[i].s; p[i].c = q[i].c; }
kernel void k4(device A* a [[buffer(0)]], constant A& c [[buffer(1)]], uint i [[thread_position_in_grid]]) { a[i].c = c.c; a[i].f = c.f; a[i].s = c.s; a[i].u = c.u; a[i].p = c.p; a[i].h = c.h; a[i].l = c.l; }
kernel void k5(device float* f [[buffer(0)]], device const uchar* bytes [[buffer(1)]], uint i [[thread_position_in_grid]]) {
    f[i] = *reinterpret_cast<device const float*>(bytes + i + 1) + float(*reinterpret_cast<device const ushort*>(bytes + 3 * i + 1));
    *reinterpret_cast<device uint*>(reinterpret_cast<device uchar*>(f) + i * 3 + 2) = 7u;
    *reinterpret_cast<device ushort*>(reinterpret_cast<device uchar*>(f) + i * 5 + 1) = ushort(i);
    *reinterpret_cast<device ulong*>(reinterpret_cast<device uchar*>(f) + i * 4) = ulong(i) << 33;
}
kernel void k6(device float4x4* m [[buffer(0)]], device const float4x4* n [[buffer(1)]], uint i [[thread_position_in_grid]]) { m[i] = transpose(n[i]) * n[i + 1]; m[i][2][i & 3] = 1.0f; }
kernel void k7(device half4x4* m [[buffer(0)]], device const half2x2* n [[buffer(1)]], uint i [[thread_position_in_grid]]) { m[i][1].xy = n[i][0]; m[i][3] = half4(n[i][1], n[i][0]); }

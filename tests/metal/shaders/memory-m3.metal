#include <metal_stdlib>
using namespace metal;
// threadgroup and private memory, function arguments by reference, pointers to pointers
struct S { float a; int b[3]; float2 c; };
static void add(thread float& x, float y) { x += y; }
static void fill(threadgroup S& s, uint i) { s.a = float(i); s.b[i % 3] = int(i); s.c = float2(i); }
static float sum(thread const float* p, int n) { float s = 0; for (int j = 0; j < n; j++) s += p[j]; return s; }
kernel void k1(device float* o [[buffer(0)]], threadgroup float* t [[threadgroup(0)]], threadgroup S* ts [[threadgroup(1)]], uint i [[thread_position_in_grid]], uint l [[thread_position_in_threadgroup]])
{
    threadgroup float shared[64];
    threadgroup S local;
    shared[l] = float(i);
    t[l] = float(l) * 2.0f;
    fill(ts[l], i);
    if (l == 0) fill(local, i);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    float x = shared[(l + 1) & 63] + t[l ^ 1];
    add(x, ts[l].c.y + float(ts[l].b[1]) + local.a);
    o[i] = x;
}
kernel void k2(device float* o [[buffer(0)]], device const float* in [[buffer(1)]], constant int& n [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    float arr[16];
    S s[4];
    for (int j = 0; j < 16; j++) arr[j] = in[j + i];
    for (int j = 0; j < 4; j++) { s[j].a = arr[j]; s[j].b[0] = j; s[j].b[1] = n; s[j].b[2] = -j; s[j].c = float2(arr[j + 4]); }
    thread float* p = &arr[i & 7];
    thread S* q = &s[i & 3];
    add(*p, 1.0f);
    o[i] = sum(arr, n & 15) + q->c.x + float(q->b[n & 1]) + p[1];
}
kernel void k3(device float* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    char c[7]; short s[5]; half h[3]; long l[2];
    for (int j = 0; j < 7; j++) c[j] = char(i + j);
    for (int j = 0; j < 5; j++) s[j] = short(i * j);
    for (int j = 0; j < 3; j++) h[j] = half(i) * half(j);
    l[0] = long(i) << 40; l[1] = -long(i);
    o[i] = float(c[i % 7]) + float(s[i % 5]) + float(h[i % 3]) + float(l[i & 1]);
}
kernel void k4(device float* o [[buffer(0)]], threadgroup char* tc [[threadgroup(0)]], threadgroup half* th [[threadgroup(1)]], threadgroup ushort* tu [[threadgroup(2)]], uint i [[thread_position_in_grid]], uint l [[thread_position_in_threadgroup]])
{
    tc[l] = char(l); th[l] = half(l); tu[l * 3 + 1] = ushort(l);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    o[i] = float(tc[l ^ 1]) + float(th[l ^ 2]) + float(tu[l]);
}

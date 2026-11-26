#include <metal_stdlib>
using namespace metal;
struct S { float4 a; packed_float3 b; half2 c; char d; ushort e; float3x3 m; int arr[5]; };
kernel void k1(device S* s [[buffer(0)]], constant S& c [[buffer(1)]], device float* f [[buffer(2)]], threadgroup float* t [[threadgroup(0)]], uint i [[thread_position_in_grid]])
{
    float loc[8];
    for (int j = 0; j < 8; j++) loc[j] = f[j + i];
    float sum = 0;
    for (int j = 0; j < 8; j++) sum += loc[(j * 3 + i) & 7];
    s[i].a = c.a * sum;
    s[i].b = c.b + packed_float3(s[i].b);
    s[i].c = c.c;
    s[i].d = c.d + 1;
    s[i].e = c.e * 2;
    s[i].m = c.m;
    s[i].arr[i & 3] = c.arr[2];
    t[i] = sum;
    f[i] = t[i + 1];
}

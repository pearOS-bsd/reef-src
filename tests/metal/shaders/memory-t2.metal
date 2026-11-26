#include <metal_stdlib>
using namespace metal;
kernel void k1(device float* a [[buffer(0)]], device float* b [[buffer(1)]], constant int& n [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    device float* p = a + i;
    float s = 0;
    for (int j = 0; j < n; j++) { s += *p; p += j + 1; }
    b[i] = s;
}
kernel void k2(device float* a [[buffer(0)]], device float* b [[buffer(1)]], constant int& n [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    device float* p = n > 2 ? a + i : b + 2 * i;
    *p = 1.0f;
}
kernel void k3(device float* a [[buffer(0)]], device float* b [[buffer(1)]], constant int& n [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    float loc[4] = {0, 0, 0, 0};
    float loc2[4]; for (int j = 0; j < 4; j++) loc2[j] = a[j];
    for (int j = 0; j < 4; j++) loc[j] += a[j * n];
    for (int j = 0; j < 4; j++) loc2[(j + n) & 3] += a[j * n];
    thread float* p = n > 2 ? loc : loc2;
    p[i & 3] = 1.0f;
    for (int j = 0; j < 4; j++) b[j] = loc[j] + loc2[j] + p[(j + i) & 3];
}
kernel void k4(device float* a [[buffer(0)]], device float* b [[buffer(1)]], constant int& n [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    device float* p = a + i;
    device float* e = a + n;
    float s = 0;
    while (p != e) { s += *p; ++p; }
    b[i] = s + (p < e ? 1 : 0);
}

#include <metal_stdlib>
using namespace metal;
// scalar and vector types through device memory
#define K(T, N) kernel void N(device T* a [[buffer(0)]], device const T* b [[buffer(1)]], constant T* c [[buffer(2)]], uint i [[thread_position_in_grid]]) { a[i] = b[i] + c[i & 7]; a[i + 3] = b[2 * i + 1]; }
K(char, kc) K(uchar, kuc) K(short, ks) K(ushort, kus) K(int, ki) K(uint, kui) K(long, kl) K(ulong, kul) K(half, kh) K(float, kf)
K(char2, kc2) K(char3, kc3) K(char4, kc4) K(uchar2, kuc2) K(uchar3, kuc3) K(uchar4, kuc4) K(short2, ks2) K(short3, ks3) K(short4, ks4)
K(ushort2, kus2) K(ushort3, kus3) K(ushort4, kus4) K(int2, ki2) K(int3, ki3) K(int4, ki4) K(uint2, kui2) K(uint3, kui3) K(uint4, kui4)
K(long2, kl2) K(long3, kl3) K(long4, kl4) K(ulong2, kul2) K(half2, kh2) K(half3, kh3) K(half4, kh4) K(float2, kf2) K(float3, kf3) K(float4, kf4)
K(packed_float2, kpf2) K(packed_float3, kpf3) K(packed_float4, kpf4) K(packed_half3, kph3) K(packed_int3, kpi3) K(packed_short3, kps3) K(packed_char3, kpc3) K(packed_uchar4, kpuc4)
kernel void kbool(device bool* a [[buffer(0)]], device const bool* b [[buffer(1)]], uint i [[thread_position_in_grid]]) { a[i] = !b[i]; a[i + 5] = b[i] && b[i + 1]; }

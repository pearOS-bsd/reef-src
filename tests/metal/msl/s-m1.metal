#include <metal_stdlib>
using namespace metal;
kernel void k(device float4 *o [[buffer(0)]], device const int4 *a [[buffer(1)]], constant half4 &h [[buffer(2)]], uint i [[thread_position_in_grid]], uint l [[thread_index_in_threadgroup]], uint3 g [[threadgroup_position_in_grid]])
{
    threadgroup float shared[64];
    int4 v = a[i];
    shared[l] = float(abs(v.x)) + float(clz(v.y)) + float(ctz(v.z)) + float(popcount(v.w)) + float(rotate(v.x, 3)) + float(max(v.x, v.y)) + float(min(uint(v.z), 5u));
    threadgroup_barrier(mem_flags::mem_threadgroup);
    float4 x = float4(shared[(l + 1) & 63]);
    x = sin(x) + cos(x) * tan(x) + exp(x) + log(abs(x) + 1) + pow(abs(x), 2.5) + sqrt(abs(x)) + rsqrt(abs(x) + 1) + floor(x) + ceil(x) + fract(x) + fmod(x, 3.0);
    x += float4(h) + clamp(x, 0.0, 1.0) + mix(x, x * 2, 0.25) + smoothstep(0.0, 1.0, x) + step(0.5, x) + sign(x) + length(x.xyz) + dot(x, x) + normalize(x + 1) + cross(x.xyz, x.zyx).xyzz;
    float4x4 m = float4x4(x, x.yzwx, x.zwxy, x.wxyz);
    o[i] = m * x + transpose(m)[1] + float4(g.x, g.y, g.z, 0) + float4(int4(x)) + float4(uint4(abs(x)));
}

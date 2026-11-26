#include <metal_stdlib>
using namespace metal;
kernel void k1(device float4 *o [[buffer(0)]], device int4 *p [[buffer(1)]], device uint4 *q [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    float4 a = o[i];
    float4 b = o[i + 1];
    o[i] = sin(a) + cos(b) + tan(a) + asin(a) + acos(b) + atan(a) + atan2(a, b) + sinh(a) + cosh(a) + tanh(b) + exp(a) + exp2(b) + exp10(a)
         + log(a) + log2(b) + log10(a) + pow(a, b) + powr(a, b) + sqrt(a) + rsqrt(b) + fabs(a) + ceil(a) + floor(b) + trunc(a) + round(b) + rint(a)
         + fract(a) + fmod(a, b) + fdim(a, b) + fmin(a, b) + fmax(a, b) + fma(a, b, a) + mix(a, b, 0.5) + clamp(a, b, a) + saturate(b) + step(a, b)
         + smoothstep(a, b, a) + sign(a) + copysign(a, b) + sinpi(a) + cospi(b) + tanpi(a) + ldexp(a, int4(2)) + nextafter(a, b) + min3(a, b, a)
         + max3(a, b, a) + median3(a, b, a) + float4(dot(a, b)) + float4(length(a)) + normalize(b) + float4(distance(a, b)) + reflect(a, b)
         + refract(a, b, 0.5) + faceforward(a, b, a) + float4(isnan(a)) + float4(isinf(b)) + float4(isfinite(a)) + float4(signbit(b)) + float4(isnormal(a))
         + float4(all(a > b)) + float4(any(a < b)) + float4(length_squared(a)) + float4(cross(a.xyz, b.xyz), 1.0);
    int4 x = p[i];
    int4 y = p[i + 1];
    p[i] = int4(abs(x)) + min(x, y) + max(x, y) + clamp(x, y, x) + int4(absdiff(x, y)) + int4(clz(x)) + int4(ctz(y)) + int4(popcount(x)) + reverse_bits(x)
         + rotate(x, y) + mulhi(x, y) + addsat(x, y) + subsat(x, y) + hadd(x, y) + rhadd(x, y) + madhi(x, y, x) + extract_bits(x, 2, 3)
         + insert_bits(x, y, 2, 3) + madsat(x, y, x);
    uint4 u = q[i];
    uint4 v = q[i + 1];
    q[i] = abs(u) + min(u, v) + max(u, v) + clamp(u, v, u) + absdiff(u, v) + clz(u) + ctz(v) + popcount(u) + reverse_bits(u) + rotate(u, v)
         + mulhi(u, v) + addsat(u, v) + subsat(u, v) + hadd(u, v) + rhadd(u, v) + madhi(u, v, u) + extract_bits(u, 2, 3) + insert_bits(u, v, 2, 3)
         + madsat(u, v, u);
}

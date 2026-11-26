//opt -nofast
#include <metal_stdlib>
using namespace metal;
kernel void k(device float4 *o [[buffer(0)]], device const float4 *in [[buffer(1)]], device const half4 *h [[buffer(2)]], device const double *d [[buffer(3)]], uint i [[thread_position_in_grid]])
{
    float4 x = in[i];
    o[i] = fma(x, x, x) + x * x + x;
    o[i + 1] = abs(x) + floor(x) + ceil(x) + trunc(x) + rint(x) + round(x) + sqrt(x) + sin(x) + cos(x) + exp(x) + exp2(x) + log(x) + log2(x) + log10(x) + pow(x, x) + copysign(x, -x) + fmin(x, 1.0f) + fmax(x, 2.0f);
    half4 y = h[i];
    o[i + 2] = float4(abs(y) + floor(y) + sqrt(y) + fma(y, y, y) + copysign(y, -y) + fmin(y, y.yxwz));
    o[i + 3] = float4(isnan(x.x) ? 1 : 0, isinf(x.y) ? 1 : 0, signbit(x.z) ? 1 : 0, isfinite(x.w) ? 1 : 0);
}

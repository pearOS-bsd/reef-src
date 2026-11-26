#include <metal_stdlib>
using namespace metal;
kernel void k(device float4 *o [[buffer(0)]], device const float4 *in [[buffer(1)]], device const int4 *ii [[buffer(2)]], uint i [[thread_position_in_grid]])
{
    float4 x = in[i];
    o[i] = fma(x, x, x) + x * x + x;
    o[i + 1] = abs(x) + floor(x) + ceil(x) + trunc(x) + rint(x) + round(x) + sqrt(x) + sin(x) + cos(x) + exp(x) + exp2(x) + log(x) + log2(x) + log10(x) + pow(x, x) + copysign(x, -x) + fmin(x, 1.0f) + fmax(x, 2.0f) + min(x, x.yzwx) + max(x, x.wxyz);
    o[i + 2] = float4(int4(x)) + float4(uint4(x)) + float4(ii[i]) + float4(uint4(ii[i]));
    o[i + 3] = precise::sin(x) + precise::fma(x, x, x) + precise::sqrt(x) + fast::sqrt(x) + fast::pow(x, 2) + fast::fmin(x, 1);
}

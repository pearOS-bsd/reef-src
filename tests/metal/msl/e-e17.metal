//opt -v 0x20004
#include <metal_stdlib>
using namespace metal;
struct Args { texture2d<float> tex [[id(0)]]; sampler smp [[id(1)]]; device float *data [[id(2)]]; constant float4 *colors [[id(3)]]; array<texture2d<float>, 3> more [[id(4)]]; float scale [[id(10)]]; };
struct Nested { Args inner; device int *counts [[id(20)]]; };
fragment float4 f(constant Args &a [[buffer(0)]], device Nested &n [[buffer(1)]], float4 p [[position]])
{
    return a.tex.sample(a.smp, p.xy) * a.scale + a.colors[0] + float4(a.data[0]) + a.more[1].sample(a.smp, p.yx) + float4(n.counts[0]) + n.inner.colors[1];
}

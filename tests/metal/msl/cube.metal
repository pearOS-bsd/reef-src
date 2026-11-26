#include <metal_stdlib>
using namespace metal;
fragment float4 f(texturecube<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t.sample(s, p.xyz) + t.read(uint2(p.xy), 2) + t.gather(s, p.xyz); }

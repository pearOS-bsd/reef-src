#include <metal_stdlib>
using namespace metal;
fragment float4 u1(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t.sample(s, p.xy); }
fragment float4 u2(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t.sample(s, p.xy) * 2; }
fragment float4 u3(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t.sample(s, p.xy) * 3; }

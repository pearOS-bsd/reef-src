#include <metal_stdlib>
using namespace metal;
constexpr sampler s(coord::normalized, filter::linear, address::repeat);
fragment float4 f(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return t.sample(s, p.xy); }

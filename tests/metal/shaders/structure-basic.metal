#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 pos [[position]]) {
    return t.sample(s, pos.xy);
}
fragment float4 f2(texture2d<float> t [[texture(0)]], float4 pos [[position]]) {
    constexpr sampler s(filter::linear, coord::pixel);
    return t.sample(s, pos.xy, level(1.0)) + t.read(uint2(pos.xy));
}

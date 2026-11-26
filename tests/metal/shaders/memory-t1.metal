#include <metal_stdlib>
using namespace metal;
kernel void k1(array<texture2d<float>, 4> tex [[texture(0)]], device float4* o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    constexpr sampler s(filter::linear);
    o[i] = tex[i & 3].sample(s, float2(0.5), level(0));
}

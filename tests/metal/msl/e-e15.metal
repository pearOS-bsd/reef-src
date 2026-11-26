#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    constexpr sampler s1(coord::normalized, address::repeat, filter::linear);
    constexpr sampler s2(coord::pixel, address::clamp_to_edge, filter::nearest, compare_func::less);
    constexpr sampler s3(mip_filter::linear, max_anisotropy(4), lod_clamp(0.0f, 5.0f));
    o[i] = 0;
}
fragment float4 f(texture2d<float> t [[texture(0)]], depth2d<float> d [[texture(1)]], float4 p [[position]])
{
    constexpr sampler s1(coord::normalized, address::repeat, filter::linear);
    constexpr sampler s2(coord::pixel, address::clamp_to_edge, filter::nearest, compare_func::less);
    constexpr sampler s3(address::mirrored_repeat, mag_filter::linear, min_filter::nearest);
    return t.sample(s1, p.xy) + t.sample(s3, p.yx) + float4(d.sample_compare(s2, p.xy, 0.5)) + t.sample(s1, p.zw);
}

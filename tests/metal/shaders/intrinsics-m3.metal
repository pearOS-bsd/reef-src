#include <metal_stdlib>
using namespace metal;
kernel void k1(texture2d<float, access::read_write> t [[texture(0)]], texture2d<float> s [[texture(1)]], sampler q [[sampler(0)]],
               device float4 *o [[buffer(0)]], uint2 i [[thread_position_in_grid]])
{
    float4 a = t.read(i);
    t.write(a * 2.0, i);
    o[i.x] = a + s.sample(q, float2(i) / 16.0, level(0)) + float4(float(s.get_width()), float(t.get_height()), 0.0, 0.0);
    t.fence();
}

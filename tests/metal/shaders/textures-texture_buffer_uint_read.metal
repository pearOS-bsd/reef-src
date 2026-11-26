#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture_buffer<uint, access::read> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint(p.x))); }
fragment float4 f2(texture_buffer<uint, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }

#include <metal_stdlib>
using namespace metal;
fragment float4 f1(depthcube_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, 3)); }
fragment float4 f2(depthcube_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xyz, 3, p.w)); }
fragment float4 f3(depthcube_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f4(depthcube_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f5(depthcube_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f6(depthcube_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f7(depthcube_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }
fragment float4 f8(depthcube_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_array_size())); }

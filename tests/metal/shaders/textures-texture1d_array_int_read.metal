#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture1d_array<int, access::read> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint(p.x), 2)); }
fragment float4 f2(texture1d_array<int, access::read> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint(p.x), 2, 3)); }
fragment float4 f3(texture1d_array<int, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f4(texture1d_array<int, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f5(texture1d_array<int, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f6(texture1d_array<int, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_array_size())); }

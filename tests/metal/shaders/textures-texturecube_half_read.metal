#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texturecube<half, access::read> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4)); }
fragment float4 f2(texturecube<half, access::read> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4, 3)); }
fragment float4 f3(texturecube<half, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f4(texturecube<half, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f5(texturecube<half, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f6(texturecube<half, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f7(texturecube<half, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }

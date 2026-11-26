#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texturecube<short, access::read_write> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4)); }
fragment float4 f2(texturecube<short, access::read_write> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4, 3)); }
fragment float4 f3(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { t.write(short4(1), uint2(p.xy), 4); return float4(0); }
fragment float4 f4(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { t.write(short4(p), uint2(p.xy), 5, 2); return float4(0); }
fragment float4 f5(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f6(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f7(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f8(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f9(texturecube<short, access::read_write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }

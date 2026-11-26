#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { t.write(ushort4(1), uint2(p.xy), 4); return float4(0); }
fragment float4 f2(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { t.write(ushort4(p), uint2(p.xy), 5, 2); return float4(0); }
fragment float4 f3(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f4(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f5(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f6(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f7(texturecube<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }

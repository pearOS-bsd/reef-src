#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { t.write(float4(1), uint3(p.xyz)); return float4(0); }
fragment float4 f2(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { t.write(float4(p), uint3(p.xyz), 2); return float4(0); }
fragment float4 f3(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f4(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f5(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f6(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f7(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }
fragment float4 f8(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_depth())); }
fragment float4 f9(texture3d<float, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_depth(1))); }

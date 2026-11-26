#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture_buffer<int, access::write> t [[texture(0)]], float4 p [[position]]) { t.write(int4(1), uint(p.x)); return float4(0); }
fragment float4 f2(texture_buffer<int, access::write> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }

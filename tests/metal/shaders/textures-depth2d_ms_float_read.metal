#include <metal_stdlib>
using namespace metal;
fragment float4 f1(depth2d_ms<float, access::read> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 1)); }
fragment float4 f2(depth2d_ms<float, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f3(depth2d_ms<float, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f4(depth2d_ms<float, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f5(depth2d_ms<float, access::read> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_samples())); }

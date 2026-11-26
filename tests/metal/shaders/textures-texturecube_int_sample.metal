#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz)); }
fragment float4 f2(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, level(1.5))); }
fragment float4 f3(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, bias(0.25))); }
vertex float4 v3(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xyz, bias(0.25))); }
kernel void k3(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xyz, bias(0.25))); }
fragment float4 f4(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, gradientcube(p.xyz, p.zyx))); }
fragment float4 f5(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xyz)); }
fragment float4 f6(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xyz, component::y)); }
vertex float4 v6(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.gather(s, p.xyz, component::y)); }
kernel void k6(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.gather(s, p.xyz, component::y)); }
fragment float4 f7(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xyz, component::z)); }
fragment float4 f8(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xyz, component::w)); }
fragment float4 f9(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4)); }
fragment float4 f10(texturecube<int, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4, 3)); }
fragment float4 f11(texturecube<int, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f12(texturecube<int, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f13(texturecube<int, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f14(texturecube<int, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f15(texturecube<int, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }

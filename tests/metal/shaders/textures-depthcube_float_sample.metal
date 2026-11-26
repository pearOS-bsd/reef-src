#include <metal_stdlib>
using namespace metal;
fragment float4 f1(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz)); }
fragment float4 f2(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, level(2.5))); }
fragment float4 f3(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xyz, p.w)); }
vertex float4 v3(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample_compare(s, p.xyz, p.w)); }
kernel void k3(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample_compare(s, p.xyz, p.w)); }
fragment float4 f4(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xyz, p.w, level(0.0))); }
fragment float4 f5(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xyz, p.w, level(3.0))); }
fragment float4 f6(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xyz)); }
vertex float4 v6(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.gather(s, p.xyz)); }
kernel void k6(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.gather(s, p.xyz)); }
fragment float4 f7(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather_compare(s, p.xyz, p.w)); }
fragment float4 f8(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4)); }
fragment float4 f9(depthcube<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 4, 3)); }
fragment float4 f10(depthcube<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f11(depthcube<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f12(depthcube<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f13(depthcube<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f14(depthcube<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }

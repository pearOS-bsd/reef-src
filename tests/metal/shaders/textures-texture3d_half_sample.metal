#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz)); }
fragment float4 f2(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, level(1.5))); }
fragment float4 f3(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, bias(0.25))); }
vertex float4 v3(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xyz, bias(0.25))); }
kernel void k3(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xyz, bias(0.25))); }
fragment float4 f4(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, gradient3d(p.xyz, p.zyx))); }
fragment float4 f5(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, int3(1, -2, 3))); }
fragment float4 f6(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, bias(-0.5), int3(1, -2, 3))); }
vertex float4 v6(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xyz, bias(-0.5), int3(1, -2, 3))); }
kernel void k6(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xyz, bias(-0.5), int3(1, -2, 3))); }
fragment float4 f7(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, level(2.0), int3(1, -2, 3))); }
fragment float4 f8(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, gradient3d(p.xyz, p.zyx), int3(1, -2, 3))); }
fragment float4 f9(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xyz, int3(0))); }
vertex float4 v9(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xyz, int3(0))); }
kernel void k9(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xyz, int3(0))); }
fragment float4 f10(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint3(p.xyz))); }
fragment float4 f11(texture3d<half, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint3(p.xyz), 3)); }
fragment float4 f12(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f13(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f14(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f15(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f16(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }
fragment float4 f17(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_depth())); }
fragment float4 f18(texture3d<half, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_depth(1))); }

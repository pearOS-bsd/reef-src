#include <metal_stdlib>
using namespace metal;
fragment float4 f1(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy)); }
fragment float4 f2(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, level(2.5))); }
fragment float4 f3(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, bias(0.5))); }
vertex float4 v3(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xy, bias(0.5))); }
kernel void k3(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xy, bias(0.5))); }
fragment float4 f4(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, gradient2d(p.xy, p.yx))); }
fragment float4 f5(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, level(1.0), int2(2, 3))); }
fragment float4 f6(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, int2(-1, 1))); }
vertex float4 v6(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xy, int2(-1, 1))); }
kernel void k6(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xy, int2(-1, 1))); }
fragment float4 f7(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, p.w)); }
fragment float4 f8(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, p.w, level(0.0))); }
fragment float4 f9(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, p.w, level(3.0))); }
vertex float4 v9(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample_compare(s, p.xy, p.w, level(3.0))); }
kernel void k9(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample_compare(s, p.xy, p.w, level(3.0))); }
fragment float4 f10(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, p.w, int2(1, -2))); }
fragment float4 f11(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, p.w, level(1.0), int2(1, -2))); }
fragment float4 f12(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy)); }
vertex float4 v12(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.gather(s, p.xy)); }
kernel void k12(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.gather(s, p.xy)); }
fragment float4 f13(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather_compare(s, p.xy, p.w)); }
fragment float4 f14(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy, int2(1, -2))); }
fragment float4 f15(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather_compare(s, p.xy, p.w, int2(1, -2))); }
vertex float4 v15(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.gather_compare(s, p.xy, p.w, int2(1, -2))); }
kernel void k15(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.gather_compare(s, p.xy, p.w, int2(1, -2))); }
fragment float4 f16(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy))); }
fragment float4 f17(depth2d<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 3)); }
fragment float4 f18(depth2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f19(depth2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f20(depth2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f21(depth2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f22(depth2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }

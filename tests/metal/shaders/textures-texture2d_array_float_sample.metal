#include <metal_stdlib>
using namespace metal;
fragment float4 f1(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3)); }
fragment float4 f2(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, level(1.5))); }
fragment float4 f3(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, bias(0.25))); }
vertex float4 v3(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xy, 3, bias(0.25))); }
kernel void k3(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xy, 3, bias(0.25))); }
fragment float4 f4(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, gradient2d(p.xy, p.yx))); }
fragment float4 f5(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, int2(1, -2))); }
fragment float4 f6(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, bias(-0.5), int2(1, -2))); }
vertex float4 v6(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xy, 3, bias(-0.5), int2(1, -2))); }
kernel void k6(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xy, 3, bias(-0.5), int2(1, -2))); }
fragment float4 f7(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, level(2.0), int2(1, -2))); }
fragment float4 f8(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, gradient2d(p.xy, p.yx), int2(1, -2))); }
fragment float4 f9(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy, 3, int2(0))); }
vertex float4 v9(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.sample(s, p.xy, 3, int2(0))); }
kernel void k9(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.sample(s, p.xy, 3, int2(0))); }
fragment float4 f10(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy, 3)); }
fragment float4 f11(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy, 3, int2(1, -2), component::y)); }
fragment float4 f12(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy, 3, int2(1, -2), component::z)); }
vertex float4 v12(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], uint vid [[vertex_id]]) { float4 p = float4(float(vid)); return float4(t.gather(s, p.xy, 3, int2(1, -2), component::z)); }
kernel void k12(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], device float4 *o [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = o[id]; o[id] = float4(t.gather(s, p.xy, 3, int2(1, -2), component::z)); }
fragment float4 f13(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy, 3, int2(1, -2), component::w)); }
fragment float4 f14(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.gather(s, p.xy, 3, int2(1, -2))); }
fragment float4 f15(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 2)); }
fragment float4 f16(texture2d_array<float, access::sample> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.read(uint2(p.xy), 2, 3)); }
fragment float4 f17(texture2d_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width())); }
fragment float4 f18(texture2d_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_num_mip_levels())); }
fragment float4 f19(texture2d_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_width(2))); }
fragment float4 f20(texture2d_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height())); }
fragment float4 f21(texture2d_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_height(1))); }
fragment float4 f22(texture2d_array<float, access::sample> t [[texture(0)]], float4 p [[position]]) { return float4(float(t.get_array_size())); }

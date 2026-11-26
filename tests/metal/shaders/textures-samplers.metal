#include <metal_stdlib>
using namespace metal;
constexpr sampler px(coord::pixel, filter::nearest);
constexpr sampler pl(coord::pixel, filter::linear, address::clamp_to_edge);
constexpr sampler nl(coord::normalized, filter::linear, mip_filter::linear, address::repeat, max_anisotropy(4), lod_clamp(1.0, 5.0));
constexpr sampler cmp(compare_func::less, filter::linear);
constexpr sampler brd(address::clamp_to_border, border_color::opaque_white, s_address::mirrored_repeat, t_address::clamp_to_zero, r_address::repeat, mag_filter::linear, min_filter::nearest);
fragment float4 s1(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return t.sample(px, p.xy) + t.sample(pl, p.xy, level(1)) + t.sample(nl, p.xy); }
fragment float4 s2(texture2d_array<float> t [[texture(0)]], float4 p [[position]]) { return t.sample(px, p.xy, 1) + t.sample(nl, p.xy, 2) + t.gather(px, p.xy, 1); }
fragment float4 s3(texture1d<float> t [[texture(0)]], texture1d_array<float> a [[texture(1)]], float4 p [[position]]) { return t.sample(px, p.x) + a.sample(px, p.x, 1) + t.sample(nl, p.x) + a.sample(nl, p.y, 2); }
fragment float4 s4(texture3d<float> t [[texture(0)]], float4 p [[position]]) { return t.sample(px, p.xyz) + t.sample(pl, p.xyz, level(2)); }
fragment float4 s5(texturecube<float> t [[texture(0)]], texturecube_array<float> a [[texture(1)]], float4 p [[position]]) { return t.sample(px, p.xyz) + a.sample(px, p.xyz, 2) + t.sample(nl, p.xyz); }
fragment float4 s6(depth2d<float> t [[texture(0)]], depth2d_array<float> a [[texture(1)]], float4 p [[position]]) { return float4(t.sample_compare(cmp, p.xy, p.z) + t.sample_compare(px, p.xy, p.w) + a.sample_compare(pl, p.xy, 1, p.w) + t.sample(brd, p.xy)) + t.gather_compare(px, p.xy, 0.5); }
fragment float4 s7(texture2d<float> t [[texture(0)]], texture2d<float> u [[texture(1)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t.sample(px, p.xy) + u.sample(px, p.xy) + t.sample(s, p.xy) + u.sample(s, p.yx) + t.sample(brd, p.zw); }
vertex float4 s8(texture2d<float> t [[texture(0)]], uint v [[vertex_id]]) { float2 p = float2(float(v)); return t.sample(px, p) + t.sample(nl, p, level(3)); }
kernel void s9(texture2d<float> t [[texture(0)]], device float4 *b [[buffer(0)]], uint id [[thread_position_in_grid]]) { b[id] = t.sample(px, b[id].xy) + t.sample(brd, b[id].zw, int2(1, 1)); }
fragment float4 s10(texture2d<float> t [[texture(0)]], sampler s [[sampler(3)]], float4 p [[position]]) { constexpr sampler local(coord::pixel); return t.sample(local, p.xy) + t.sample(s, p.xy, min_lod_clamp(2.0)); }

#include <metal_stdlib>
using namespace metal;
struct Args { texture2d<float> t [[id(0)]]; sampler s [[id(1)]]; depth2d<float> d [[id(2)]]; texture2d<float, access::write> w [[id(3)]]; texture_buffer<float> b [[id(4)]]; };
fragment float4 b1(constant Args &a [[buffer(0)]], float4 p [[position]]) { a.w.write(p, uint2(p.xy)); return a.t.sample(a.s, p.xy) + float4(a.d.sample_compare(a.s, p.xy, p.z)) + a.t.read(uint2(p.xy)) + float4(a.t.get_width()) + a.b.read(uint(p.x)); }
fragment float4 b2(constant Args &a [[buffer(0)]], texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return a.t.sample(s, p.xy) + t.sample(a.s, p.xy) + t.sample(s, p.xy); }
struct Many { array<texture2d<float>, 3> t [[id(0)]]; sampler s [[id(3)]]; };
fragment float4 b3(constant Many &m [[buffer(0)]], float4 p [[position]]) { return m.t[0].sample(m.s, p.xy) + m.t[2].sample(m.s, p.xy); }
fragment float4 b4(array<texture2d<float>, 2> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t[0].sample(s, p.xy) + t[1].sample(s, p.xy); }
fragment float4 b5(array<texture2d<float>, 2> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t[int(p.x)].sample(s, p.xy); }
fragment float4 b6(texture2d<float> t [[texture(0)]], texture2d<float> unused [[texture(1)]], sampler s [[sampler(0)]], float4 p [[position]]) { return t.sample(s, p.xy); }

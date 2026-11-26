#include <metal_stdlib>
using namespace metal;
struct Inner { float4 color [[id(0)]]; device float *data [[id(1)]]; };
struct Resources {
    float scale [[id(0)]];
    int4 counts [[id(1)]];
    float4x4 matrix [[id(2)]];
    array<float, 3> weights [[id(3)]];
    texture2d<float> plain [[id(6)]];
    texture2d<half, access::read> halves [[id(7)]];
    texture2d<uint, access::write> uints [[id(8)]];
    texture3d<int> ints [[id(9)]];
    texture1d_array<short> shorts [[id(10)]];
    texturecube<ushort> ushorts [[id(11)]];
    depth2d<float> depth [[id(12)]];
    depthcube_array<float> depths [[id(13)]];
    texture2d_ms<float> multi [[id(14)]];
    texture_buffer<float> tbuffer [[id(15)]];
    array<texture2d<float>, 4> many [[id(16)]];
    array<depth2d_array<float>, 2> depthArrays [[id(20)]];
    sampler smp [[id(22)]];
    array<sampler, 2> samplers [[id(23)]];
    device float *buffer [[id(25)]];
    constant Inner *inner [[id(26)]];
    texture2d_ms_array<float> multiArray [[id(27)]];
    texture2d_array<float> plainArray [[id(28)]];
    texture1d<float> oned [[id(29)]];
    texturecube_array<float> cubes [[id(30)]];
};
kernel void k(constant Resources &r [[buffer(1)]], device float *out [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    out[i] = r.scale + r.weights[1] + r.plain.read(uint2(0)).x + float(r.halves.read(uint2(0)).x) + r.buffer[i] + r.inner->color.x
           + r.many[2].read(uint2(1)).x + float(r.ints.read(uint3(0)).x) + r.depth.read(uint2(0));
}
fragment float4 f(constant Resources &r [[buffer(3)]], float4 p [[position]]) { return r.plain.read(uint2(p.xy)) * r.scale; }

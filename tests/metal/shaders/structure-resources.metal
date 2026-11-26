#include <metal_stdlib>
using namespace metal;

constant float table[8] = {1, 2, 3, 4, 5, 6, 7, 8.5};

kernel void r_buffers(device float4 *a [[buffer(0)]], constant float4 *b [[buffer(1)]], device const int *c [[buffer(2)]],
                      device uint *d [[buffer(5)]], uint id [[thread_position_in_grid]], uint n [[threads_per_grid]])
{
    a[id] = b[id] * table[id & 7] + float4(c[id]);
    d[id] = n;
}

kernel void r_shared(device float *a [[buffer(0)]], threadgroup float *t [[threadgroup(0)]], uint lid [[thread_position_in_threadgroup]],
                     uint id [[thread_position_in_grid]])
{
    threadgroup float local[64];
    local[lid] = a[id];
    t[lid] = a[id] * 2.0;
    threadgroup_barrier(mem_flags::mem_threadgroup);
    a[id] = local[63 - lid] + t[(lid + 1) & 63];
}

kernel void r_private(device int *a [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    int v[6];
    int w[5];
    for (int i = 0; i < 6; ++i)
        v[i] = 5 - i * a[0];
    for (int i = 0; i < 5; ++i)
        w[i] = 0;
    for (int i = 0; i < 6; ++i) {
        v[i] += a[i + id];
        w[i % 5] += v[(i + id) % 6];
    }
    a[id] = v[a[id] & 3] + w[id % 5];
}

kernel void r_textures(texture2d<float, access::read> in [[texture(0)]], texture2d<float, access::write> out [[texture(1)]],
                       texture2d<int, access::read_write> rw [[texture(2)]], texture3d<uint> vol [[texture(3)]],
                       texturecube<float> cube [[texture(4)]], depth2d<float> shadow [[texture(5)]], sampler s [[sampler(0)]],
                       texture2d_array<float> arr [[texture(6)]], uint2 id [[thread_position_in_grid]])
{
    constexpr sampler cmp(compare_func::less, filter::linear);
    float4 v = in.read(id);
    int4 r = rw.read(id);
    rw.write(r + 1, id);
    uint4 q = vol.sample(s, float3(0.5));
    float4 c = cube.sample(s, float3(0, 1, 0), level(0));
    float d = shadow.sample_compare(cmp, float2(0.5), 0.3, level(0));
    float4 a = arr.sample(s, float2(0.25), 2, level(0));
    out.write(v + float4(q) + c + d + a, id);
}

fragment float4 r_fetch(float4 fb [[color(0)]], uint4 fbu [[color(1)]], float4 pos [[position]]) { return fb * 0.5 + float4(fbu) + pos; }

struct Material { texture2d<float> albedo [[id(0)]]; sampler s [[id(1)]]; constant float4 *scale [[id(2)]]; };
fragment float4 r_argument(constant Material &m [[buffer(0)]], float4 pos [[position]])
{
    return m.albedo.sample(m.s, pos.xy) * m.scale[0];
}

kernel void r_address(device uint *a [[buffer(0)]], device uint *b [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    device uint *p = (id & 1) ? a : b;
    p[id] = uint(ulong(p) & 0xff);
}

fragment float4 r_samplers(texture2d<float> t [[texture(0)]], sampler s1 [[sampler(0)]], sampler s2 [[sampler(1)]], float4 pos [[position]])
{
    constexpr sampler lin(filter::linear, address::repeat);
    return t.sample(s1, pos.xy) + t.sample(s2, pos.yx) + t.sample(lin, pos.zw) + t.read(uint2(pos.xy));
}

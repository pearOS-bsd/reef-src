#include <metal_stdlib>
using namespace metal;
kernel void k(device uint *o [[buffer(0)]], device const uint *in [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    uint x = in[i];
    o[i] = (x << 3) | (x >> 29);
    o[i + 1] = (x >> 5) | (x << 27);
    ushort s = ushort(x);
    o[i + 2] = ushort((s << 4) | (s >> 12));
    o[i + 3] = (x & 0xff) << 24 | (x & 0xff00) << 8 | (x >> 8 & 0xff00) | x >> 24;
    o[i + 4] = popcount(x) + clz(x) + ctz(x) + reverse_bits(x) + abs(int(x)) + max(x, 3u) + min(int(x), -2) + addsat(x, 7u) + subsat(int(x), 9) + mulhi(x, 3u) + madhi(x, 3u, 1u);
    o[i + 5] = rotate(x, 7u) + extract_bits(x, 3, 5) + insert_bits(x, 5u, 2, 3);
    ulong l = in[i + 1]; o[i + 6] = uint((l << 13) | (l >> 51));
}

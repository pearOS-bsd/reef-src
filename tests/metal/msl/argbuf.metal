#include <metal_stdlib>
using namespace metal;
struct Inner { float a; int b[3]; };
struct Args {
    texture2d<float> tex [[id(0)]];
    sampler smp [[id(1)]];
    device float *data [[id(2)]];
    constant Inner *inner [[id(3)]];
    float4 color [[id(4)]];
    Inner nested [[id(5)]];
    array<texture2d<float>, 4> texs [[id(10)]];
    texture2d<float, access::write> out [[id(14)]];
};
kernel void k1(device Args &args [[buffer(0)]], constant float4 &c [[buffer(1)]], uint id [[thread_position_in_grid]]) {
    args.data[id] = args.tex.sample(args.smp, float2(0.5)).x + args.color.x + args.inner->a + args.nested.b[1] + args.texs[2].read(uint2(0)).x + c.y;
    args.out.write(float4(1), uint2(id));
}

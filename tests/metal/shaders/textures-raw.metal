#include <metal_stdlib>
using namespace metal;
typedef __texture::handle H;
typedef __texture::sampler_handle S;
float4 plain2(H, S, float2) __asm__("air.sample_texture_2d.v4f32");
float2 plain2v2(H, S, float2) __asm__("air.sample_texture_2d.v2f32");
float3 plain2v3(H, S, float2) __asm__("air.sample_texture_2d.v3f32");
float plain2f(H, S, float2) __asm__("air.sample_texture_2d.f32");
int4 plain2i(H, S, float2) __asm__("air.sample_texture_2d.s.v4i32");
uint2 plain2u2(H, S, float2) __asm__("air.sample_texture_2d.u.v2i32");
short4 plain2s(H, S, float2) __asm__("air.sample_texture_2d.s.v4i16");
ushort plain2us(H, S, float2) __asm__("air.sample_texture_2d.u.i16");
float4 compare4(H, S, float2, float) __asm__("air.sample_compare_depth_2d.v4f32");
float2 compare2(H, S, float2, float) __asm__("air.sample_compare_depth_2d.v2f32");
float compare1(H, S, float, float) __asm__("air.sample_compare_depth_2d.f32");
float4 offset2(H, S, float2, bool, int2) __asm__("air.sample_texture_2d.v4f32");
float4 offset2s(H, S, float2, bool, int) __asm__("air.sample_texture_2d.v4f32");
float4 offset2l(H, S, float2, bool, long2) __asm__("air.sample_texture_2d.v4f32");
float4 gather2(H, S, float2, bool, int2, int) __asm__("air.gather_texture_2d.v4f32");
float4 gather2n(H, S, float2) __asm__("air.gather_texture_2d.v4f32");
float4 level2(H, S, float2, bool, int2, bool, float, float, int) __asm__("air.sample_texture_2d.v4f32");
float4 level2short(H, S, float2, bool, int2, bool) __asm__("air.sample_texture_2d.v4f32");
float4 level2nomode(H, S, float2, bool, int2, bool, float) __asm__("air.sample_texture_2d.v4f32");
float4 level2min(H, S, float2, bool, int2, bool, float, float) __asm__("air.sample_texture_2d.v4f32");
float4 level2int(H, S, float2, bool, int2, bool, float, int) __asm__("air.sample_texture_2d.v4f32");
float4 grad2(H, S, float2, float2, float2) __asm__("air.sample_texture_2d_grad.v4f32");
float4 grad2min(H, S, float2, float2, float2, float) __asm__("air.sample_texture_2d_grad.v4f32");
float4 grad2b(H, S, float2, float2, float2, bool, int2) __asm__("air.sample_texture_2d_grad.v4f32");
float4 few(H) __asm__("air.sample_texture_2d.v4f32");
float4 few2(H, S) __asm__("air.sample_texture_2d.v4f32");
uint query(H) __asm__("air.get_foo_texture_2d");
uint query2(H, uint) __asm__("air.get_depth_texture_2d");
uint query3(H) __asm__("air.get_num_samples_texture_2d");
uint query4(H) __asm__("air.get_width_something");
uint4 query5(H) __asm__("air.get_width_texture_2d");
S readSampler() __asm__("air.get_read_sampler");
float4 readS(H, S, uint2, uint2, uint) __asm__("air.read_texture_2d.v4f32");
float4 readSi(H, S, uint2, int2, uint) __asm__("air.read_texture_2d.v4f32");
float4 readS2(H, S, uint2) __asm__("air.read_texture_2d.v4f32");
float4 readArr(H, S, uint2, uint2, uint, uint) __asm__("air.read_texture_2d_array.v4f32");
float4 read2(H, uint2) __asm__("air.read_texture_2d.v4f32");
void write2(H, uint2, float2, uint) __asm__("air.write_texture_2d.v4f32");
void write3(H, uint2, float3) __asm__("air.write_texture_2d.v4f32");
void writef(H, uint2, float) __asm__("air.write_texture_2d.v4f32");
void writei(H, uint2, int4) __asm__("air.write_texture_2d.s.v4i32");
void writeu(H, uint2, uint4) __asm__("air.write_texture_2d.u.v4i32");
void writes(H, uint2, short4) __asm__("air.write_texture_2d.s.v4i16");
void writeus(H, uint2, ushort4) __asm__("air.write_texture_2d.u.v4i16");
float4 nosampler(H, H, float2) __asm__("air.sample_texture_2d.v4f32");
float cubeArrayGrad(H, S, float3, uint, float, float3, float3) __asm__("air.sample_compare_depth_cube_array_grad.f32");
float4 onBuffer(device float *, S, float2) __asm__("air.sample_texture_2d.v4f32");
float4 cube(H, S, float3) __asm__("air.sample_texture_cube.v4f32");
float4 cubeArray(H, S, float3, uint) __asm__("air.sample_texture_cube_array.v4f32");
float4 arr2(H, S, float2, uint) __asm__("air.sample_texture_2d_array.v4f32");
float4 one(H, S, float) __asm__("air.sample_texture_1d.v4f32");
float4 oneArr(H, S, float, uint) __asm__("air.sample_texture_1d_array.v4f32");
float4 three(H, S, float3) __asm__("air.sample_texture_3d.v4f32");
float4 compareCubeArray(H, S, float3, uint, float) __asm__("air.sample_compare_depth_cube_array.v4f32");
float compareCubeArrayLevel(H, S, float3, uint, float, bool, float, float, int) __asm__("air.sample_compare_depth_cube_array.f32");

fragment float4 r1(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return plain2(t.__h, s.__handle(), p.xy); }
fragment float4 r2(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2v2(t.__h, s.__handle(), p.xy), 0, 1); }
fragment float4 r3(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2v3(t.__h, s.__handle(), p.xy), 1); }
fragment float4 r4(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2f(t.__h, s.__handle(), p.xy)); }
fragment float4 r5(texture2d<int> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2i(t.__h, s.__handle(), p.xy)); }
fragment float4 r6(texture2d<uint> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(float2(plain2u2(t.__h, s.__handle(), p.xy)), 0, 0); }
fragment float4 r7(texture2d<short> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(plain2s(t.__h, s.__handle(), p.xy)); }
fragment float4 r8(texture2d<ushort> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(float(plain2us(t.__h, s.__handle(), p.xy))); }
fragment float4 r9(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return compare4(t.__h, s.__handle(), p.xy, p.z); }
fragment float4 r10(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compare2(t.__h, s.__handle(), p.xy, p.z), 0, 0); }
fragment float4 r11(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compare1(t.__h, s.__handle(), p.x, p.z)); }
fragment float4 r12(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2(t.__h, s.__handle(), p.xy, true, int2(p.xy)); }
fragment float4 r13(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2(t.__h, s.__handle(), p.xy, true, int2(3, -4)); }
fragment float4 r14(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2s(t.__h, s.__handle(), p.xy, true, 5); }
fragment float4 r15(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return offset2l(t.__h, s.__handle(), p.xy, true, long2(5, 6)); }
fragment float4 r16(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return gather2(t.__h, s.__handle(), p.xy, true, int2(0), int(p.x)); }
fragment float4 r17(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return gather2n(t.__h, s.__handle(), p.xy); }
fragment float4 r18(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), p.x > 0.5, 1.0, 0.0, 1); }
fragment float4 r19(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), true, 1.0, 0.0, int(p.x)); }
fragment float4 r20(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2short(t.__h, s.__handle(), p.xy, true, int2(0), true); }
fragment float4 r21(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2nomode(t.__h, s.__handle(), p.xy, true, int2(1), true, 2.0); }
fragment float4 r22(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2min(t.__h, s.__handle(), p.xy, true, int2(1), true, 2.0, 1.5); }
fragment float4 r23(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2int(t.__h, s.__handle(), p.xy, true, int2(1), true, 2.0, 1); }
fragment float4 r24(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), true, p.x, p.y, 1); }
fragment float4 r25(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), true, p.x, p.y, 0); }
fragment float4 r26(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), false, p.x, p.y, 0); }
fragment float4 r27(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), true, p.x, 0.0, 2); }
fragment float4 r28(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return grad2(t.__h, s.__handle(), p.xy, p.xy, p.yx); }
fragment float4 r29(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return grad2min(t.__h, s.__handle(), p.xy, p.xy, p.yx, p.z); }
fragment float4 r30(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return grad2b(t.__h, s.__handle(), p.xy, p.xy, p.yx, true, int2(1, 2)); }
fragment float4 r31(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return few(t.__h); }
fragment float4 r32(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return few2(t.__h, s.__handle()); }
fragment float4 r33(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query(t.__h)); }
fragment float4 r34(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query2(t.__h, 1)); }
fragment float4 r35(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query3(t.__h)); }
fragment float4 r36(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query4(t.__h)); }
fragment float4 r37(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return float4(query5(t.__h)); }
fragment float4 r38(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return readS(t.__h, readSampler(), uint2(p.xy), uint2(1, 2), 0); }
fragment float4 r39(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return readS(t.__h, readSampler(), uint2(p.xy), uint2(0), 1); }
fragment float4 r40(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return readSi(t.__h, readSampler(), uint2(p.xy), int2(1, 2), 0); }
fragment float4 r41(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return readS2(t.__h, readSampler(), uint2(p.xy)); }
fragment float4 r42(texture2d_array<float> t [[texture(0)]], float4 p [[position]]) { return readArr(t.__h, readSampler(), uint2(p.xy), uint2(p.yx), 3, 1); }
fragment float4 r43(texture2d<float> t [[texture(0)]], float4 p [[position]]) { return read2(t.__h, uint2(p.xy)); }
fragment float4 r44(texture2d<float, access::write> t [[texture(0)]], float4 p [[position]]) { write2(t.__h, uint2(p.xy), p.xy, 0); return 0; }
fragment float4 r45(texture2d<float, access::write> t [[texture(0)]], float4 p [[position]]) { write3(t.__h, uint2(p.xy), p.xyz); return 0; }
fragment float4 r46(texture2d<float, access::write> t [[texture(0)]], float4 p [[position]]) { writef(t.__h, uint2(p.xy), p.x); return 0; }
fragment float4 r47(texture2d<int, access::write> t [[texture(0)]], float4 p [[position]]) { writei(t.__h, uint2(p.xy), int4(p)); return 0; }
fragment float4 r48(texture2d<uint, access::write> t [[texture(0)]], float4 p [[position]]) { writeu(t.__h, uint2(p.xy), uint4(p)); return 0; }
fragment float4 r49(texture2d<short, access::write> t [[texture(0)]], float4 p [[position]]) { writes(t.__h, uint2(p.xy), short4(p)); return 0; }
fragment float4 r50(texture2d<ushort, access::write> t [[texture(0)]], float4 p [[position]]) { writeus(t.__h, uint2(p.xy), ushort4(p)); return 0; }
fragment float4 r51(texture2d<float> t [[texture(0)]], texture2d<float> u [[texture(1)]], float4 p [[position]]) { return nosampler(t.__h, u.__h, p.xy); }
fragment float4 r52(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(cubeArrayGrad(t.__h, s.__handle(), p.xyz, 2, p.w, p.xyz, p.zyx)); }
fragment float4 r53(device float *b [[buffer(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return onBuffer(b, s.__handle(), p.xy); }
fragment float4 r54(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], sampler z [[sampler(1)]], float4 p [[position]]) { return t.sample(s, p.xy) + t.sample(z, p.yx) + t.sample(s, p.zw) + t.read(uint2(p.xy)) + float4(t.get_width()); }
fragment float4 r55(depth2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample(s, p.xy) + t.sample_compare(s, p.xy, p.z) + t.sample(s, p.yx) + t.sample_compare(s, p.yx, p.w)); }
vertex float4 r56(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], uint v [[vertex_id]]) { float4 p = float4(float(v)); return float4(t.sample_compare(s, p.xyz, 2, p.w)) + float4(t.sample(s, p.xyz, 1)); }
fragment float4 r57(texture2d<float, access::sample> t [[texture(0)]], float4 p [[position]]) { t.write(float4(1), uint2(p.xy)); return 0; }
fragment float4 r58(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return compareCubeArray(t.__h, s.__handle(), p.xyz, 2, p.w); }
fragment float4 r59(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compareCubeArrayLevel(t.__h, s.__handle(), p.xyz, 2, p.w, true, 1.0, 0.0, 1)); }
fragment float4 r60(depthcube_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(compareCubeArrayLevel(t.__h, s.__handle(), p.xyz, 2, p.w, true, 1.0, 0.5, 0)); }
fragment float4 r61(depth2d_array<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return float4(t.sample_compare(s, p.xy, 1, p.w, level(2.0)) + t.sample_compare(s, p.xy, 1, p.w)); }
vertex float4 r62(depth2d_array<float> t [[texture(0)]], depthcube<float> c [[texture(1)]], sampler s [[sampler(0)]], uint v [[vertex_id]]) { float4 p = float4(float(v)); return float4(t.sample_compare(s, p.xy, 1, p.w) + c.sample_compare(s, p.xyz, p.w) + c.sample_compare(s, p.xyz, p.w, level(1.0))); }
kernel void r63(texturecube<float> t [[texture(0)]], texture1d<float> o [[texture(1)]], texture3d<float> d [[texture(2)]], sampler s [[sampler(0)]], device float4 *b [[buffer(0)]], uint id [[thread_position_in_grid]]) { float4 p = b[id]; b[id] = cube(t.__h, s.__handle(), p.xyz) + one(o.__h, s.__handle(), p.x) + three(d.__h, s.__handle(), p.xyz); }
fragment float4 r64(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(0), true, p.x, p.y, 3); }
fragment float4 r65(texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]], float4 p [[position]]) { return level2(t.__h, s.__handle(), p.xy, true, int2(2, 2), true, p.x, p.y, 0) + level2(t.__h, s.__handle(), p.xy, true, int2(2, 2), true, p.x, p.y, 1); }

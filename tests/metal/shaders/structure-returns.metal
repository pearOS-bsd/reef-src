#include <metal_stdlib>
using namespace metal;

struct VOut { float4 pos [[position]]; float2 uv; int flag; float size [[point_size]]; };

vertex VOut v_early(uint vid [[vertex_id]], constant float4 *p [[buffer(0)]], constant int &mode [[buffer(1)]])
{
    VOut o;
    o.pos = p[vid];
    o.uv = p[vid].xy * 0.5;
    o.flag = 0;
    o.size = 1.0;
    if (mode == 1) {
        o.flag = 7;
        return o;
    }
    if (mode == 2) {
        o.pos = float4(0.0);
        o.size = 4.0;
        return o;
    }
    for (int i = 0; i < mode; ++i) {
        o.uv += p[i].zw;
        if (o.uv.x > 10.0)
            return o;
    }
    o.flag = mode;
    return o;
}

struct FOut { float4 color [[color(0)]]; int4 ids [[color(1)]]; uint4 more [[color(2)]]; float depth [[depth(greater)]]; };

fragment FOut f_paths(VOut in [[stage_in]], constant float4 &tint [[buffer(0)]])
{
    FOut o;
    o.color = tint;
    o.ids = int4(in.flag);
    o.more = uint4(1, 2, 3, 4);
    o.depth = 0.5;
    if (in.flag == 3)
        discard_fragment();
    if (in.uv.x < 0.0) {
        o.color = float4(1, 0, 0, 1);
        if (in.uv.y < 0.0)
            return o;
        o.depth = 0.75;
    } else if (in.uv.y > 1.0) {
        o.color.a = 0.0;
    }
    return o;
}

fragment float4 f_chain(float4 pos [[position]], constant float *c [[buffer(0)]])
{
    float a = pos.x;
    if (c[0] > 0.0) { if (c[1] > 0.0) return float4(1.0); a = sin(a) * cos(pos.y) + c[2] * c[3] - c[4] / (c[5] + 1.0) + exp(c[6]) - log(c[7] + 2.0) + sqrt(abs(c[8])) * c[9]; }
    a = a * a + c[10] * a + c[11] - pow(abs(a), 0.3) + fract(a * 1.7) + floor(a * 0.3) + c[12] * c[13] * c[14] + tan(c[15]) + atan2(c[16], c[17] + 1.0);
    if (c[18] > 0.0) { if (c[19] > 0.0) return float4(2.0); a = sin(a) * cos(pos.y) + c[20] * c[21] - c[22] / (c[23] + 1.0) + exp(c[24]) - log(c[25] + 2.0) + sqrt(abs(c[26])) * c[27]; }
    a = a * a + c[28] * a + c[29] - pow(abs(a), 0.3) + fract(a * 1.7) + floor(a * 0.3) + c[30] * c[31] * c[32] + tan(c[33]) + atan2(c[34], c[35] + 1.0);
    if (c[36] > 0.0) { if (c[37] > 0.0) return float4(3.0); a = sin(a) * cos(pos.y) + c[38] * c[39] - c[40] / (c[41] + 1.0) + exp(c[42]) - log(c[43] + 2.0) + sqrt(abs(c[44])) * c[45]; }
    a = a * a + c[46] * a + c[47] - pow(abs(a), 0.3) + fract(a * 1.7) + floor(a * 0.3) + c[48] * c[49] * c[50] + tan(c[51]) + atan2(c[52], c[53] + 1.0);
    if (c[54] > 0.0) { if (c[55] > 0.0) return float4(4.0); a = sin(a) * cos(pos.y) + c[56] * c[57] - c[58] / (c[59] + 1.0) + exp(c[60]) - log(c[61] + 2.0) + sqrt(abs(c[62])) * c[63]; }
    a = a * a + c[64] * a + c[65] - pow(abs(a), 0.3) + fract(a * 1.7) + floor(a * 0.3) + c[66] * c[67] * c[68] + tan(c[69]) + atan2(c[70], c[71] + 1.0);
    return float4(a);
}

fragment float4 f_select(float4 pos [[position]], constant int &m [[buffer(0)]])
{
    bool a = pos.x > 1.0 && pos.y < 2.0;
    bool b = pos.z > 0.5 || m == 3;
    float r = a ? pos.x : (b ? pos.y : pos.z);
    if (a && (b || m > 7))
        r += 1.0;
    return float4(r, a ? 1.0 : 0.0, b ? 1.0 : 0.0, 1.0);
}

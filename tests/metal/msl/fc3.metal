#include <metal_stdlib>
using namespace metal;
constant bool useTex [[function_constant(0)]];
constant bool useTwo [[function_constant(1)]];
constant int count [[function_constant(3)]];
constant float scale [[function_constant(4)]];
constant float4 tint [[function_constant(5)]];
constant bool hasCount = is_function_constant_defined(count);
struct VOut { float4 pos [[position]]; float2 uv [[user(uv)]]; float extra [[user(e), function_constant(useTwo)]]; };
vertex VOut v1(uint vid [[vertex_id]], constant float4 *p [[buffer(0)]], constant float *q [[buffer(1)]]) {
    VOut o; o.pos = p[vid] * (hasCount ? count : 1) * scale; o.uv = float2(0);
    if (useTwo) o.extra = q[0];
    return o;
}
fragment float4 f1(VOut in [[stage_in]], texture2d<float> t [[texture(0)]], sampler s [[sampler(0)]]) {
    float4 c = tint;
    if (useTex) c *= t.sample(s, in.uv);
    return c;
}

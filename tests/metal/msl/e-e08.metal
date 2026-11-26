#include <metal_stdlib>
using namespace metal;
struct VOut { float4 p [[position]]; float4 c; };
vertex VOut v(uint id [[vertex_id]]) { VOut o; o.p = float4(id); o.c = 1; return o; }
fragment float4 f(VOut in [[stage_in]], device float *b [[buffer(0)]], texture2d<float> t [[texture(0)]], texture2d<float> t2 [[texture(0)]]) { return in.c; }

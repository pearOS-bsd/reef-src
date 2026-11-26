#include <metal_stdlib>
using namespace metal;
constexpr float table[3] = {sin(1.0f), cos(2.0f), 3};
constant float other = table[1] * 2;
fragment float4 f(float4 p [[position]]) { return float4(table[0], table[2], other, p.x); }

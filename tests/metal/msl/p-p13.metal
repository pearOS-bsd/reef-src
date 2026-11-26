#include <metal_stdlib>
using namespace metal;
struct H { half4 h; int2 n; H() { h.w = 1.0h; h.x = 2.0h; h.y = 0.5h; h.z = 0.25h; n.y = 7; n.x = 9; } };
constant H hv;
fragment half4 f(float4 p [[position]]) { return hv.h + half4(half(hv.n.x), half(hv.n.y), 0, 0); }

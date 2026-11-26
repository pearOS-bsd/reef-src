#include <metal_stdlib>
using namespace metal;
vertex float4 v(uint id [[vertex_id]], uint id2 [[vertex_id]]) { return float4(id); }
fragment float4 f(float4 p [[position]], float4 q [[position]]) { return p; }
kernel void k(uint a [[thread_position_in_grid]], uint b [[thread_position_in_grid]]) {}

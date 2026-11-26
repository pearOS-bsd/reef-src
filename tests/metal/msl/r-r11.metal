#include <metal_stdlib>
using namespace metal;
struct U { float v; float a() const volatile device { return v; } float b() volatile const thread const { return v; } };
kernel void k(device U *u [[buffer(0)]], device float *o [[buffer(1)]], uint i [[thread_position_in_grid]]) { U l = {3}; o[i] = u[i].a() + l.b(); }

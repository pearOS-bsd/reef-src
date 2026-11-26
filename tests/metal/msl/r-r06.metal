#include <metal_stdlib>
using namespace metal;
struct T {
    int n;
    int count() constant { return n; }
    int count() threadgroup { return n + 1; }
};
kernel void k(constant T &t [[buffer(0)]], device int *o [[buffer(1)]], uint i [[thread_position_in_grid]]) { o[i] = t.count(); }

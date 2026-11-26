#include <metal_stdlib>
using namespace metal;
[[visible]] float visible_one(float x) { return x * 3; }
[[visible]] int visible_two(int a, int b);
[[stitchable]] float stitch(float x, float y) { return x + y; }
[[visible]] float visible_one(float x);
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = visible_one(o[i]); }

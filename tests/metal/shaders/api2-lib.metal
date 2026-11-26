#include <metal_stdlib>
using namespace metal;
[[visible]] float scale(float x) { return x * 2.0f; }
[[visible]] int twice(int x) { return x * 2; }
float helper(float x) { return x + 1.0f; }
[[visible]] float shifted(float x) { return helper(x) * 3.0f; }

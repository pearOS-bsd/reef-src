#include <metal_stdlib>
using namespace metal;
int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }
kernel void k(device int *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = fib(int(i)); }

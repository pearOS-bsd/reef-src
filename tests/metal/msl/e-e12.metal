#include <metal_stdlib>
using namespace metal;
constant int table[4] = {1, 2, 3, 4};
device int *global_pointer;
kernel void k(device int *o [[buffer(0)]], uint i [[thread_position_in_grid]]) { o[i] = table[i & 3]; }

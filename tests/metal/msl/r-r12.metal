#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]] [[ ]], uint j [[ thread_index_in_threadgroup ]])
{
    float arr[2] = {1, 2};
    o[i] = arr[ [](){ return 0; }() ] + float(j);
}

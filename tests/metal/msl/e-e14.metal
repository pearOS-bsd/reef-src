#include <metal_stdlib>
using namespace metal;
constant float scale [[function_constant(0)]];
constant bool use_scale = is_function_constant_defined(scale);
constant int count [[function_constant(1)]];
constant half hh [[function_constant(2)]];
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    o[i] = use_scale ? scale : 1.0f;
    if (is_function_constant_defined(count)) o[i] += count;
    o[i] += float(hh);
}

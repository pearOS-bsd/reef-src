#include <metal_stdlib>
using namespace metal;

constant int other [[function_constant(5)]];
constant int count [[function_constant(3)]];
constant half hscale [[function_constant(6)]];
constant bool opt = is_function_constant_defined(hscale);

kernel void k3(device int *out [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    if (is_function_constant_defined(other))
        out[id] = other;
    else
        out[id] = count;
}

kernel void k5(device half *out [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    out[id] = opt ? hscale : half(count);
}

#include <metal_stdlib>
using namespace metal;

constant bool useA [[function_constant(0)]];
constant bool useB [[function_constant(1)]];
constant bool useC [[function_constant(2)]];
constant int count [[function_constant(3)]];
constant float scale [[function_constant(4)]];
constant int derived = count * 3 + 1;
constant bool either = useA || useC;
constant bool scaled = is_function_constant_defined(scale);

kernel void k1(device float *out [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    float v = out[id];
    if (useA)
        v += 1.0;
    if (useB)
        v *= 2.0;
    if (useC)
        v -= 3.0;
    for (int i = 0; i < count; ++i)
        v += float(i);
    out[id] = v + float(derived) + (either ? 1.0 : 0.0);
}

kernel void k2(device float *out [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    if (is_function_constant_defined(scale))
        out[id] *= scale;
    else
        out[id] += 1.0;
    if (scaled)
        out[id] += 2.0;
}

kernel void k4(device int *out [[buffer(0)]], device float *f [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    out[id] = derived;
    f[id] = useC ? 0.5 : 1.5;
}

#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    auto f = [](float x) thread { return x * 2; };
    auto g = [=](float x) mutable { return x + 1; };
    auto h = [](float x) noexcept { return x - 1; };
    o[i] = f(o[i]) + g(1) + h(2);
}

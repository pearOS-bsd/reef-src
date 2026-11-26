#include <metal_stdlib>
using namespace metal;
struct S {
    float v;
    float get() device { return v * 2; }
    float get() thread { return v; }
    float twice() const device { return v * 4; }
};
kernel void k(device S *s [[buffer(0)]], device float *o [[buffer(1)]], uint i [[thread_position_in_grid]])
{
    S local = {1};
    o[i] = s[i].get() + local.get() + s[i].twice();
}

#include <metal_stdlib>
using namespace metal;
kernel void k(device float *o [[buffer(31)]], device float *p [[buffer(40)]], texture2d<float> t [[texture(200)]], sampler s [[sampler(20)]]) { o[0] = p[0]; }

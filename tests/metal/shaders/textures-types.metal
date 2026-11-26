#include <metal_stdlib>
using namespace metal;
struct texture4d { __texture::handle __h; };
fragment float4 y1(texture4d t [[texture(0)]], float4 p [[position]]) { return float4(1); }

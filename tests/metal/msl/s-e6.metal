#include <metal_stdlib>
using namespace metal;
struct S { float a; int b[4]; float3 c; };
kernel void k(device S *s [[buffer(0)]], constant S &c [[buffer(1)]], device void *raw [[buffer(2)]], uint i [[thread_index_in_simdgroup]]) [[max_total_threads_per_threadgroup(256)]]
{ s[i].a = c.a + c.b[i & 3] + c.c.x; ((device int *)raw)[i] = i; }
[[early_fragment_tests]] fragment half4 f(float4 p [[position]], float2 pc [[point_coord]]) { return half4(p.x, pc.x, pc.y, 1); }

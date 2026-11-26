#include <metal_stdlib>
using namespace metal;
struct ICB { indirect_command_buffer cmd [[id(0)]]; };
kernel void k1(device ICB& icb [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    compute_command c(icb.cmd, i);
    c.set_threadgroups_per_grid(uint3(1, 1, 1));
}
struct P { compute_pipeline_state p [[id(0)]]; indirect_command_buffer cmd [[id(1)]]; };
kernel void k2(device P& a [[buffer(0)]], uint i [[thread_position_in_grid]])
{
    compute_command c(a.cmd, i);
    c.set_compute_pipeline_state(a.p);
}

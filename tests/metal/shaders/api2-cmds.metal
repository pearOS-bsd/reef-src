#include <metal_stdlib>
using namespace metal;

struct Commands { command_buffer cmds [[id(0)]]; };
struct States { render_pipeline_state render [[id(0)]]; compute_pipeline_state compute [[id(1)]]; command_buffer more [[id(2)]]; };

kernel void icb_encode(device Commands &c [[buffer(0)]], constant States &s [[buffer(4)]], uint id [[thread_position_in_grid]])
{
    compute_command cmd(c.cmds, id);
    cmd.set_compute_pipeline_state(s.compute);
    render_command r(s.more, id);
    r.set_render_pipeline_state(s.render);
}

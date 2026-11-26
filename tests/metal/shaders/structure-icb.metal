#include <metal_stdlib>
using namespace metal;

struct Commands { command_buffer cmds [[id(0)]]; };

kernel void icb_encode(device Commands &c [[buffer(0)]], device float *data [[buffer(1)]], constant uint &n [[buffer(2)]],
                       uint id [[thread_position_in_grid]])
{
    compute_command cmd(c.cmds, id);
    if (id < n) {
        cmd.set_kernel_buffer(data, 1);
        cmd.concurrent_dispatch_threads(uint3(64, 1, 1), uint3(64, 1, 1));
    } else {
        cmd.reset();
    }
}

struct Draw { command_buffer cmds [[id(0)]]; };

kernel void icb_draw(device Draw &d [[buffer(0)]], device float4 *verts [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    render_command cmd(d.cmds, id);
    for (uint i = 0; i < 2; ++i) {
        if (verts[i].x > 0.0) {
            cmd.set_vertex_buffer(verts, 0);
            cmd.draw_primitives(primitive_type::triangle, 0, 3, 1, 0);
            return;
        }
    }
    cmd.reset();
}

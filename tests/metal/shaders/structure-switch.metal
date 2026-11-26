#include <metal_stdlib>
using namespace metal;

kernel void s_basic(device int *o [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    int r;
    switch (o[id]) {
    case 0: r = 5; break;
    case 1: r = o[1] + 2; break;
    case 2:
    case 3: r = o[2] * o[3]; break;
    case 10: r = -1; break;
    case 100: r = o[100] ^ 77; break;
    default: r = 42; break;
    }
    o[id] = r;
}

kernel void s_fall(device uint *o [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    uint r = 0;
    switch (o[id] & 7) {
    case 0: r += o[10];
    case 1: r += o[11] * 3;
    case 2: r ^= 0x55;
        break;
    case 5: return;
    case 6: r = 6;
    default: r += 1;
    }
    o[id] = r;
}

kernel void s_loop(device int *o [[buffer(0)]], constant int &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    int s = 0;
    for (int i = 0; i < n; ++i) {
        switch (o[i]) {
        case 1: continue;
        case 2: s += 2; break;
        case 3: s *= 3; break;
        case 4: goto done;
        default: s -= o[i];
        }
        s += 1;
    }
done:
    o[id] = s;
}

kernel void s_long(device long *o [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    long v = o[id];
    long r = 0;
    switch (v) {
    case 0x100000000: r = 1; break;
    case -5: r = 2; break;
    case 7: r = 3; break;
    default: r = v * 2;
    }
    o[id] = r;
}

kernel void s_short(device ushort *o [[buffer(0)]], uint id [[thread_position_in_grid]])
{
    ushort v = o[id];
    switch (v) {
    case 65535: o[id] = 1; break;
    case 3: o[id] = 2; break;
    case 300: o[id] = v + 2; break;
    }
}

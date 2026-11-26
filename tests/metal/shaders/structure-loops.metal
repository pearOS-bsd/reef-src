#include <metal_stdlib>
using namespace metal;

kernel void k_for(device int *o [[buffer(0)]], constant int &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    int s = 0;
    for (int i = 0; i < n; ++i) {
        if (i == 7)
            continue;
        if (s > 1000)
            break;
        s += i * o[i];
    }
    o[id] = s;
}

kernel void k_nested(device int *o [[buffer(0)]], constant int &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    int s = 0;
    bool done = false;
    for (int i = 0; i < n && !done; ++i) {
        for (int j = 0; j < n; ++j) {
            if (o[i * n + j] < 0) {
                done = true;
                break;
            }
            if (o[j] == 3)
                continue;
            s += o[i * n + j];
        }
    }
    o[id] = s;
}

kernel void k_exits(device int *o [[buffer(0)]], constant int &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    int i = 0;
    int r = 0;
    while (true) {
        int v = o[i];
        if (v == 1) { r = 10; break; }
        if (v == 2) { r = 20 + i; break; }
        if (v == 3) { o[id] = i * 3; return; }
        if (v == 4) { r = o[i + 1] * 7; break; }
        ++i;
        if (i >= n) { r = -1; break; }
    }
    o[id + 1] = r * 2;
    o[id + 2] = r ^ 5;
}

kernel void k_do(device float *o [[buffer(0)]], constant uint &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    float a = 1.0, b = 2.0;
    uint i = 0;
    do {
        float t = a;
        a = b;
        b = t + b * 0.5;
        ++i;
    } while (i < n && a < 100.0);
    o[id] = a + b;
}

kernel void k_swap(device uint *o [[buffer(0)]], constant uint &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    uint a = o[0], b = o[1], c = o[2];
    for (uint i = 0; i < n; ++i) {
        uint t = a;
        a = b;
        b = c;
        c = t;
    }
    o[id] = a * 100 + b * 10 + c;
}

kernel void k_deep(device int *o [[buffer(0)]], constant int &n [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    int s = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            for (int k = j; k < n; ++k) {
                if (o[k] == 9)
                    goto out;
                if (o[k] == 8)
                    break;
                s += o[i] * o[j] - o[k];
            }
            if (s > 50)
                continue;
            s ^= j;
        }
    }
out:
    o[id] = s;
}

kernel void k_whilebreak(device float4 *o [[buffer(0)]], constant float &limit [[buffer(1)]], uint id [[thread_position_in_grid]])
{
    float4 p = o[id];
    int steps = 0;
    while (length(p) < limit) {
        p = p * 1.5 + 0.25;
        if (p.x > 3.0 && p.y < 1.0)
            break;
        if (++steps > 64)
            return;
    }
    o[id] = p;
}

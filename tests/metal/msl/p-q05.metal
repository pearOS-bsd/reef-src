#include <metal_stdlib>
using namespace metal;
struct Node { int value; int next; };
int walk(device const Node *n, int start) { int s = 0; for (int i = start; i >= 0; i = n[i].next) s += n[i].value; return s; }
int deep(int x) { if (x <= 0) return 0; return x + deep(x - 1) * 2; }
kernel void k(device int *o [[buffer(0)]], device const Node *n [[buffer(1)]], uint i [[thread_position_in_grid]]) { o[i] = walk(n, int(i)) + deep(int(i)); }

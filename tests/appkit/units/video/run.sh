#!/bin/bash
# In the container: run.sh NAME [driver options and case files]: the same with the original and with ours; compares the logs.
# STUB=stub_noexport (...) takes another set of stub libraries.
O=/out/video/test; cd $O; n=$1; shift
export LD_LIBRARY_PATH=$O/${STUB:-stub}:/root2
nice ./drv_orig $n.orig.log "$@" & nice ./drv_ours $n.ours.log "$@" & wait
if cmp -s $n.orig.log $n.ours.log; then echo "$n: same, $(wc -l < $n.orig.log) lines, $(wc -c < $n.orig.log) bytes"; else echo "$n: DIFFERENT, $(diff $n.orig.log $n.ours.log | grep -c '^<') lines"; diff $n.orig.log $n.ours.log | cut -c1-200 | head -${HEAD:-10}; fi

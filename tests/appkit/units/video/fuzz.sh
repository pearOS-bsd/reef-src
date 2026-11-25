#!/bin/bash
# In the container: fuzz.sh FIRST COUNT SHARDS case...: run.sh --fuzz in parallel, counts what is different.
first=$1; count=$2; shards=$3; shift 3
for i in $(seq 0 $((shards-1))); do bash /tk/app/video/test/run.sh fuzz$i --fuzz $((first+i*count)) $count "$@" & done; wait
cd /out/video/test
for i in $(seq 0 $((shards-1))); do diff fuzz$i.orig.log fuzz$i.ours.log | grep '^[<>]' ; done | sort -k2n | head -40
cat fuzz?.orig.log | awk '{e[$3" "$4]++; s+=$7} END {for (k in e) print k, e[k]; print s, "pictures"}'

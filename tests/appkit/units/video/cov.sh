#!/bin/bash
# In the container: the lines of ours that the cases do not reach.
T=/tk/app/video/test; O=/out/video/cov; rm -rf $O; mkdir -p $O; cd $O
for f in /w/src/appkit-host/video*.cpp; do g++ -std=c++20 -g -O0 --coverage -w -I/tk/tk7/src -c -o $(basename $f .cpp).o $f & done; wait
g++ -std=c++20 -g -O1 -w -rdynamic -DCOVERAGE -c -o drv.o $T/drv.cpp && g++ --coverage -rdynamic -o drv drv.o video*.o -ldl || exit 1
export LD_LIBRARY_PATH=/out/video/test/stub
S=$T/streams; C="$S/h264_main.case $S/hevc_main10.case $S/vp9_p2.case $S/hevc_scaling.case"
./drv a.log --every 2 $S/*.case; ./drv a.log --every 3 $T/synth/*.case $T/synth2/*.case
for f in GetDisplayDRM Initialize QueryConfigProfiles QueryConfigEntrypoints GetConfigAttributes CreateConfig CreateContext CreateSurfaces CreateBuffer BeginPicture SyncSurface QueryImageFormats CreateImage DeriveImage GetImage MapBuffer ExportSurfaceHandle; do ./drv a.log --fail $f:1 --every 3 $C;  ./drv a.log --fail $f:0 --every 3 $C; done
for o in node=129 node=136 profiles=0 vld=0 rtformat=0 formats=0 layers=5; do ./drv a.log --option $o --every 3 $C; done
LD_LIBRARY_PATH=/out/video/test/stub_noexport ./drv a.log $C; LD_LIBRARY_PATH=/out/video/test/stub_noderive ./drv a.log --option formats=0 $C;  LD_LIBRARY_PATH=/none ./drv a.log $C
./drv a.log --fuzz 1 ${1:-1500} $S/*.case $T/synth/*.case $T/synth2/*.case
gcov -r -s /w/src/appkit-host video*.gcda 2>/dev/null | grep -A1 "^File" | grep -v "^--"
for g in video*.gcov; do grep -H "#####" $g; done | cut -c1-160

#!/bin/bash
# In the container: everything, one after the other. Output: one line per run.
T=/tk/app/video/test; S=$T/streams
bash $T/build.sh > /dev/null 2>&1
bash $T/run.sh all --every 2 $S/*.case
bash $T/synthrun.sh synth; bash $T/synthrun.sh synth2 synth2
bash $T/run.sh keep0 --keep 0 $S/h264_high_auto.case $S/hevc_refs.case $S/vp9_altref_split.case $S/h264_mmco.case
bash $T/run.sh keep9 --keep 9 --every 5 $S/h264_high_auto.case $S/hevc_refs.case $S/vp9_altref_split.case $S/hevc_main10.case
for d in 2 3 5 7; do bash $T/run.sh drop$d --drop $d --every 9 $S/*.case; done
C="$S/h264_main.case $S/hevc_main10.case $S/vp9_p2.case $S/hevc_scaling.case"
for f in GetDisplayDRM Initialize MaxNumProfiles QueryConfigProfiles MaxNumEntrypoints QueryConfigEntrypoints GetConfigAttributes CreateConfig CreateContext CreateSurfaces CreateBuffer BeginPicture RenderPicture EndPicture SyncSurface MaxNumImageFormats QueryImageFormats CreateImage DeriveImage GetImage MapBuffer ExportSurfaceHandle; do
  for n in 0 1 3; do bash $T/run.sh fail_${f}_$n --fail $f:$n --every 3 $C; done
done 2>&1 | awk '/same/{n++; l+=$3} !/same/{print} END {print n, "runs with a failing call: same,", l, "lines"}'
for o in node=129 node=135 node=136 profiles=0 profiles=0x40 profiles=0x7f vld=0 rtformat=1 rtformat=0x100 rtformat=0 formats=0 formats=1 formats=2 formats=4 planes=1 planes=3 pitch=0 pitch=1000 objects=2 objects=4 layers=0 layers=1 layers=3 layers=4 layers=5 derived=0x30313050 derived=0x32315659; do
  bash $T/run.sh option_$o --option $o --every 3 $C
done 2>&1 | awk '/same/{n++; l+=$3} !/same/{print} END {print n, "runs with another driver: same,", l, "lines"}'
bash $T/run.sh derive --option formats=0 --every 1 $C
for s in stub_noexport stub_noderive stub_nocallbacks; do STUB=$s bash $T/run.sh $s --every 3 $C; done
STUB=stub_noderive bash $T/run.sh noderive --option formats=0 --every 3 $C
STUB=none bash $T/run.sh nolibrary $S/h264_main.case
echo done

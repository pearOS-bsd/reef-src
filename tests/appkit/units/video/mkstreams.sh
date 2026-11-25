#!/bin/bash
# On the host: makes the streams with ffmpeg and turns them into case files (streams/*.case).
cd "$(dirname "$0")"
mkdir -p streams
F="/usr/bin/ffmpeg -hide_banner -loglevel error -y"
src() { echo "-f lavfi -i testsrc2=size=$1:rate=25:duration=${2:-2}"; }
h264() { n=$1; s=$2; shift 2; [ -s streams/$n.mp4 ] || $F $(src $s $DUR) -pix_fmt ${FMT:-yuv420p} -c:v libx264 "$@" streams/$n.mp4 || echo "failed $n"; }
hevc() { n=$1; s=$2; p=$3; shift 3; [ -s streams/$n.mp4 ] || $F $(src $s $DUR) -pix_fmt ${FMT:-yuv420p} -c:v libx265 -tag:v hvc1 -x265-params "log-level=error:$p" "$@" streams/$n.mp4 || echo "failed $n"; }
vp9() { n=$1; s=$2; shift 2; [ -s streams/$n.mp4 ] || $F $(src $s $DUR) -pix_fmt ${FMT:-yuv420p} -c:v libvpx-vp9 -deadline realtime -cpu-used 6 "$@" streams/$n.mp4 || echo "failed $n"; }

h264 h264_base 320x240 -profile:v baseline -x264-params ref=1:bframes=0
h264 h264_base_refs 320x240 -profile:v baseline -x264-params ref=6:bframes=0:keyint=20
h264 h264_main 352x288 -profile:v main -x264-params ref=3:bframes=2
h264 h264_main_cavlc 352x288 -profile:v main -vf fade=in:0:30 -x264-params cabac=0:weightp=2:weightb=1:bframes=3:b-pyramid=normal:ref=5
h264 h264_high_strict 352x288 -profile:v high -x264-params bframes=5:b-pyramid=strict:ref=8:direct=temporal
h264 h264_high_spatial 640x360 -profile:v high -x264-params bframes=5:b-pyramid=normal:ref=16:direct=spatial:b-adapt=0
h264 h264_high_auto 640x360 -profile:v high -vf fade=out:20:20 -x264-params bframes=8:b-pyramid=normal:ref=4:direct=auto:weightp=2:b-adapt=0
h264 h264_mmco 320x240 -profile:v main -x264-params bframes=3:b-pyramid=normal:ref=2:b-adapt=0
h264 h264_cqm_jvt 320x240 -profile:v high -x264-params cqm=jvt
h264 h264_cqm_custom 320x240 -profile:v high -x264-params "cqm4iy=8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23:cqm8p=9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40"
h264 h264_slices 640x360 -profile:v high -x264-params slices=4:deblock=2,-3
h264 h264_slicesize 640x360 -profile:v main -x264-params slice-max-size=800:no-deblock=1
h264 h264_odd 178x142 -profile:v high
h264 h264_odd2 130x66 -profile:v baseline -x264-params keyint=5
h264 h264_opengop 320x240 -profile:v high -x264-params open-gop=1:keyint=12:bframes=3
h264 h264_intrarefresh 320x240 -profile:v high -x264-params intra-refresh=1:keyint=10:bframes=0
h264 h264_aud_hrd 320x240 -profile:v high -x264-params aud=1:nal-hrd=vbr:vbv-maxrate=1000:vbv-bufsize=1000:constrained-intra=1:chroma-qp-offset=-3
h264 h264_fake 320x240 -x264-params fake-interlaced=1
h264 h264_interlaced 320x240 -x264-params tff=1
FMT=yuv422p h264 h264_422 320x240
FMT=yuv444p h264 h264_444 320x240
FMT=yuv420p10le h264 h264_high10 320x240
FMT=gray h264 h264_gray 320x240
h264 h264_intra 320x240 -profile:v high -x264-params keyint=1
h264 h264_lossless 320x240 -qp 0
h264 h264_repeat 320x240 -profile:v high -x264-params repeat-headers=1:keyint=8:bframes=2
DUR=5 h264 h264_long 320x240 -profile:v main -x264-params ref=3:bframes=2:keyint=250

hevc hevc_main 352x288 "bframes=3:ref=3"
hevc hevc_nob 320x240 "bframes=0:ref=1"
hevc hevc_refs 640x360 "bframes=8:ref=6:b-adapt=0:b-pyramid=1:rd=3"
hevc hevc_weight 352x288 "weightp=1:weightb=1:bframes=3:ref=4" -vf fade=in:0:30
hevc hevc_scaling 320x240 "scaling-list=default"
hevc hevc_wpp 640x360 "wpp=1:slices=1"
hevc hevc_nowpp 640x360 "wpp=0"
hevc hevc_slices 640x480 "slices=4:wpp=1:pools=4:frame-threads=1"
hevc hevc_opengop 320x240 "open-gop=1:keyint=12:min-keyint=12:bframes=4"
hevc hevc_closed 320x240 "open-gop=0:keyint=12:min-keyint=12:bframes=4"
hevc hevc_temporal 320x240 "temporal-layers=3:bframes=3:b-adapt=0"
hevc hevc_ctu16 178x142 "ctu=16:min-cu-size=8"
hevc hevc_ctu32 320x240 "ctu=32:max-tu-size=16:tu-inter-depth=3:tu-intra-depth=3"
hevc hevc_tools 320x240 "amp=1:rect=1:tskip=1:constrained-intra=1:cbqpoffs=2:crqpoffs=-3:strong-intra-smoothing=1:cu-lossless=1:limit-modes=0"
hevc hevc_nofilter 320x240 "deblock=0:no-deblock=1:sao=0"
hevc hevc_deblock 320x240 "deblock=3,-2:sao=1:limit-sao=1"
hevc hevc_lossless 320x240 "lossless=1"
hevc hevc_repeat 320x240 "repeat-headers=1:aud=1:keyint=8:hrd=1:vbv-maxrate=1000:vbv-bufsize=1000"
hevc hevc_optpps 640x360 "opt-qp-pps=1:opt-ref-list-length-pps=1:bframes=4:ref=4"
hevc hevc_optrps 640x360 "multi-pass-opt-rps=1:bframes=4:ref=4"
hevc hevc_odd 130x66 "keyint=5"
hevc hevc_intra 320x240 "keyint=1"
hevc hevc_signhide 320x240 "signhide=0:aq-mode=0:cutree=0:tmvp=0"
hevc hevc_eos 320x240 "eob=1:eos=1:keyint=10"
FMT=yuv420p10le hevc hevc_main10 352x288 "bframes=3:ref=3"
FMT=yuv420p10le hevc hevc_main10_scaling 352x288 "scaling-list=default:weightp=1" -vf fade=in:0:30
FMT=yuv420p12le hevc hevc_main12 320x240 "bframes=0"
FMT=yuv422p hevc hevc_422 320x240 "bframes=0"
FMT=yuv444p hevc hevc_444 320x240 "bframes=0"
FMT=gray hevc hevc_gray 320x240 "bframes=0"
DUR=5 hevc hevc_long 320x240 "bframes=4:ref=4:keyint=250"

vp9 vp9_basic 320x240
vp9 vp9_altref 352x288 -deadline good -cpu-used 4 -auto-alt-ref 1 -lag-in-frames 25 -b:v 200k
vp9 vp9_altref6 352x288 -deadline good -cpu-used 4 -auto-alt-ref 6 -lag-in-frames 25 -b:v 100k -arnr-maxframes 7
vp9 vp9_odd 320x240 -vf scale=179:143
vp9 vp9_odd2 320x240 -vf scale=33:65
vp9 vp9_tiny 8x8
vp9 vp9_one 320x240 -vf scale=1:1
FMT=yuv420p10le vp9 vp9_odd10 320x240 -vf scale=177:143
vp9 vp9_resilient 320x240 -error-resilient 1
vp9 vp9_aq1 320x240 -deadline good -cpu-used 5 -aq-mode 1
vp9 vp9_aq2 320x240 -deadline good -cpu-used 5 -aq-mode 2 -b:v 100k
vp9 vp9_aq3 320x240 -aq-mode 3 -b:v 100k -lag-in-frames 0
vp9 vp9_lossless 320x240 -lossless 1
vp9 vp9_tiles 1280x720 -tile-columns 2 -tile-rows 2 -frame-parallel 1 -row-mt 1
vp9 vp9_wide 2176x144 -tile-columns 3
vp9 vp9_svc 320x240 -lag-in-frames 0 -ts-parameters "ts_number_layers=3:ts_target_bitrate=100,200,300:ts_rate_decimator=4,2,1:ts_periodicity=4:ts_layer_id=0,2,1,2:ts_layering_mode=3"
vp9 vp9_key 320x240 -g 5 -keyint_min 5
vp9 vp9_sharp 320x240 -sharpness 5 -b:v 60k
vp9 vp9_screen 320x240 -tune-content screen
FMT=yuv420p10le vp9 vp9_p2 320x240
FMT=yuv420p12le vp9 vp9_p2_12 320x240
FMT=yuv444p vp9 vp9_p1 320x240
FMT=yuv422p10le vp9 vp9_p3 320x240
ls streams/*.mp4 | wc -l
python3 mp4cases.py streams

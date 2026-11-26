#!/bin/sh
# Run inside the test container: Calculator in a headless sway, screenshot at the end.
# usage: window-test.sh [SECS] [OUT]   (needs sway, grim; Reef root /tmp/calcroot)
SECS=${1:-30}; OUT=${2:-/tmp/calc-shots}
rm -rf $OUT; mkdir -p $OUT
export XDG_RUNTIME_DIR=/tmp/xdg WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 WLR_RENDERER=pixman HOME=/root
[ -d /root2 ] && export LD_LIBRARY_PATH=/root2
mkdir -p $XDG_RUNTIME_DIR; chmod 700 $XDG_RUNTIME_DIR
pkill -x sway; sleep 1
echo 'output HEADLESS-1 resolution 1280x800 scale 1' > /tmp/sway.conf
sway -c /tmp/sway.conf >$OUT/sway.log 2>&1 &
sleep 3
export WAYLAND_DISPLAY=wayland-1 SWAYSOCK=$(ls $XDG_RUNTIME_DIR/sway-ipc.*.sock | head -1)
( timeout -s TERM $SECS /tmp/calcroot/bin/reef /root/Applications/Calculator.app >$OUT/stdout 2>$OUT/stderr; echo $? >$OUT/status ) &
RP=$!
for t in 10 $((SECS-6)); do sleep $((t - ${T:-0})); T=$t; grim -t png $OUT/shot$t.png 2>/dev/null; swaymsg -t get_tree > $OUT/tree$t.json 2>/dev/null; done
wait $RP
pkill -x sway

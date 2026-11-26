#!/bin/sh
# Run inside the test container: Calculator in a headless sway, type the given keys with wtype, screenshot at the end.
# usage: window-keys.sh OUT "key key ..."   keys: a character, or Return, Escape, BackSpace ...
OUT=$1; KEYS=$2
rm -rf $OUT; mkdir -p $OUT
export XDG_RUNTIME_DIR=/tmp/xdg WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 WLR_RENDERER=pixman HOME=/root REEF_LOG_DISPLAY=1
[ -d /root2 ] && export LD_LIBRARY_PATH=/root2
mkdir -p $XDG_RUNTIME_DIR; chmod 700 $XDG_RUNTIME_DIR
pkill -x sway; sleep 1
echo 'output HEADLESS-1 resolution 1280x800 scale 1' > /tmp/sway.conf
sway -c /tmp/sway.conf >$OUT/sway.log 2>&1 &
sleep 3
export WAYLAND_DISPLAY=wayland-1 SWAYSOCK=$(ls $XDG_RUNTIME_DIR/sway-ipc.*.sock | head -1)
( REEF_INJECT_KEYS="$KEYS" timeout -s TERM 60 /tmp/calcroot/bin/reef /root/Applications/Calculator.app >$OUT/stdout 2>$OUT/stderr ) &
sleep $((10 + $(echo $KEYS | wc -w)))
grim -t png $OUT/final.png
pkill -f calcroot/bin/reef; pkill -x sway

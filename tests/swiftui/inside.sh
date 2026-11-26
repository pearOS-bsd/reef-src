#!/bin/sh
# Runs inside the Reef test container (headless sway + grim): inside.sh APP.app OUT [TIMEOUT]
# Starts the app, waits for the line "FINAL <name>" on stdout (or TIMEOUT seconds), then captures
# a screenshot and the sway window tree, and stops the app. Files in OUT: stdout stderr status
# shot.ppm tree.json final (1 if the final line was seen) missing.json
APP=$1; OUT=$2; TMO=${3:-40}
rm -rf "$OUT"; mkdir -p "$OUT"
export XDG_RUNTIME_DIR=/run/user/0 WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 WLR_RENDERER=pixman
mkdir -p $XDG_RUNTIME_DIR; chmod 700 $XDG_RUNTIME_DIR
pkill -x sway; pkill -f -- --mach-broker; pkill -f /Applications/; sleep 1
rm -rf /run/user/0/* /root/h /Applications; mkdir -p /root/h /Applications
export HOME=/root/h
name=$(basename "$APP"); id=pear.swiftuitest.${name%.app}
cp -a "$APP" "/Applications/$name"
# the original run leaves "reopen windows" state after a crash: always start clean
rm -rf "$HOME/Library/Saved Application State/$id.savedState"
echo 'output HEADLESS-1 resolution 1280x800 scale 1' > /tmp/sway.conf
sway -c /tmp/sway.conf >"$OUT/sway.log" 2>&1 &
sleep 3
export WAYLAND_DISPLAY=wayland-1 SWAYSOCK=$(ls $XDG_RUNTIME_DIR/sway-ipc.*.sock | head -1)
reef --list-missing=json "/Applications/$name" > "$OUT/missing.json" 2>/dev/null
export REEF_DUMP_VIEWS=${DUMPSECS:-8}
( timeout -s TERM $TMO reef "/Applications/$name" >"$OUT/stdout" 2>"$OUT/stderr"; echo $? >"$OUT/status" ) &
RP=$!
i=0; echo 0 > "$OUT/final"
while [ $i -lt $((TMO*2)) ]; do
  if grep -q '^FINAL ' "$OUT/stdout" 2>/dev/null; then echo 1 > "$OUT/final"; break; fi
  [ -e "$OUT/status" ] && break
  sleep 0.5; i=$((i+1))
done
# let the dump (REEF_DUMP_VIEWS) and rendering settle, then capture
sleep $(( ${DUMPSECS:-8} > 3 ? 4 : 2 ))
grim -t ppm "$OUT/shot.ppm" 2>/dev/null
swaymsg -t get_tree > "$OUT/tree.json" 2>/dev/null
pkill -f "/Applications/$name" ; kill $RP 2>/dev/null
wait $RP 2>/dev/null
[ -e "$OUT/status" ] || echo killed > "$OUT/status"
pkill -x sway; pkill -f -- --mach-broker
rm -rf "$HOME/Library/Saved Application State/$id.savedState"

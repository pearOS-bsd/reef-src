# In the container: build.sh first. Runs the driver with the published library and with ours, compares the logs.
cd /tmp/dialogs/t; mkdir -p home
export LD_LIBRARY_PATH=/root2 HOME=/tmp/dialogs/t/home XDG_RUNTIME_DIR=/tmp/dialogs/t/home LC_ALL=C.UTF-8
ulimit -v ${LIMIT:-6000000}
for w in orig ours; do LD_PRELOAD=${PRE:+$PRE:}/tmp/dialogs/t/hooks.so timeout 300 ./driver $w > $w.log 2> $w.err; echo $w $?; tail -1 $w.log | grep -qx end || { echo "FAILED: $w did not reach the end"; tail -n 3 $w.log $w.err; bad=1; }; done
[ -z "$bad" ] || exit 1
wc -l orig.log ours.log; cmp orig.log ours.log && echo LOGS SAME || diff orig.log ours.log | head -${N:-40}; tail -2 orig.log; head -5 orig.err ours.err

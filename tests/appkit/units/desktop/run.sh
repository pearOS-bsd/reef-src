# In the container: builds, runs the scenarios with the original and with ours, compares the logs. run.sh [scenario...]
bash /tk/app/desktop/test/build.sh >/dev/null || exit 1
cd /tmp/desktop/t; rm -f /tmp/desktop/driver.err
timeout 1500 python3 -B /tk/app/desktop/test/client.py /o/lib/cutefish-reef/host/libreef-appkit-host.so ./driver orig.log "$@"; echo orig $?
timeout 1500 python3 -B /tk/app/desktop/test/client.py /tmp/desktop/t/ours.so ./driver ours.log "$@"; echo ours $?
wc -l orig.log ours.log
cmp orig.log ours.log && echo LOGS SAME || diff orig.log ours.log | head -${N:-40}

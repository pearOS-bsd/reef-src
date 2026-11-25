# In the container: builds the driver, runs the scenario on the original and on ours, compares the logs.
bash /tk/app/menu_bus/test/build.sh || exit 1
cd /tmp/dbt
timeout 400 python3 /tk/app/menu_bus/test/client.py /tmp/dbt/orig.log ./driver orig /o/lib/cutefish-reef/host/libreef-appkit-host.so 2>/dev/null; echo orig $?
timeout 400 python3 /tk/app/menu_bus/test/client.py /tmp/dbt/ours.log ./driver 2>/dev/null; echo ours $?
wc -l orig.log ours.log; cmp orig.log ours.log && echo LOGS SAME
python3 /tk/app/menu_bus/test/monitor.py orig.log.monitor > orig.mon; python3 /tk/app/menu_bus/test/monitor.py ours.log.monitor > ours.mon
wc -l orig.mon ours.mon; cmp orig.mon ours.mon && echo MONITOR SAME

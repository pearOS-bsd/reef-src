# In the container: ours with coverage, and ours with the sanitizers
mkdir -p /tmp/dbt/cov && cd /tmp/dbt
SAN="--coverage -O0" SUF=.cov bash /tk/app/menu_bus/test/build.sh >/dev/null 2>&1
rm -f *.gcda
timeout 500 python3 /tk/app/menu_bus/test/client.py /tmp/dbt/cov.log ./driver.cov 2>/dev/null; echo cov $?
cmp orig.log cov.log && echo COV LOG SAME
gcov -b -c menu_bus.o now_playing.o status_item.o 2>/dev/null | grep -A4 "File '/w/src/appkit-host" 
grep -nE "#####" menu_bus.cpp.gcov now_playing.cpp.gcov status_item.cpp.gcov menu_bus.h.gcov now_playing.h.gcov 2>/dev/null | head -40

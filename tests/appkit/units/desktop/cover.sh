# In the container: ours with coverage over all scenarios (COVERAGE: the driver leaves by exit)
export SUF=cov; D=/tmp/desktop/tcov; mkdir -p $D; cd $D; rm -f *.gcda
SAN="--coverage -O0" bash /tk/app/desktop/test/build.sh >/dev/null 2>&1
export COVERAGE=1
timeout 1500 python3 -B /tk/app/desktop/test/client.py $D/ours.so $D/driver cov.log "$@"; echo cov $?
[ -e ../t/orig.log ] && cmp ../t/orig.log cov.log && echo COV LOG SAME
gcov -b -c desktop.o 2>/dev/null | grep -A5 "File '/w/src/appkit-host/desktop"
grep -n "#####" desktop.cpp.gcov | head -30
grep -nE "branch +[0-9]+ (never executed|taken 0)" desktop.cpp.gcov | wc -l

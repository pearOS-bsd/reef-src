# In the container: ours with the sanitizers
mkdir -p /tmp/dbt && cd /tmp/dbt
SAN="-fsanitize=address,undefined -fno-omit-frame-pointer -O1" SUF=.san bash /tk/app/menu_bus/test/build.sh 2>&1 | tail -3
ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1 UBSAN_OPTIONS=print_stacktrace=1 timeout 900 python3 /tk/app/menu_bus/test/client.py /tmp/dbt/san.log ./driver.san 2>san.err; echo san $?
cmp orig.log san.log && echo SAN LOG SAME; diff orig.log san.log | head
grep -E "ERROR|runtime error|SUMMARY|leak" san.err | head -20

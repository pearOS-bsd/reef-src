# In the container: ours with the address and undefined behaviour sanitizers over all scenarios
export SUF=san; D=/tmp/desktop/tsan; mkdir -p $D; cd $D
F="-fsanitize=address,undefined -fno-omit-frame-pointer -O1"
FL="-std=c++20 -g -fPIC -fvisibility=hidden -w -I/tk/tk7/src -I/tk/app/stubs -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz $F"
for u in application desktop now_playing; do g++ $FL -c -o $u.o /w/src/appkit-host/$u.cpp & done
g++ $FL -c -o export.o /tk/app/desktop/test/export.cpp &
g++ -std=c++20 -g -w $F -rdynamic -o driver /tk/app/desktop/test/driver.cpp -ldl &
gcc -shared -fPIC -O1 -o fakehome.so /tk/app/desktop/test/fakehome.c &
wait
g++ -shared $F -o ours.so application.o desktop.o now_playing.o export.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,--unresolved-symbols=ignore-all 2>&1 | grep -v warning
rm -f /tmp/desktop/driver.err
ASAN_OPTIONS=verify_asan_link_order=0:detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1 timeout 1500 python3 -B /tk/app/desktop/test/client.py $D/ours.so $D/driver san.log "$@"; echo san $?
[ -e ../t/orig.log ] && cmp ../t/orig.log san.log && echo SAN LOG SAME
grep -E "ERROR|runtime error|SUMMARY" /tmp/desktop/driver.err | sort | uniq -c | head -20

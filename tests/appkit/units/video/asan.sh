#!/bin/bash
# In the container: ours with the address sanitizer on the streams and the synthetic cases; prints what it finds.
T=/tk/app/video/test; O=/out/video/asan; rm -rf $O; mkdir -p $O; cd $O
for f in /w/src/appkit-host/video*.cpp; do g++ -std=c++20 -g -O1 -fsanitize=address -fno-omit-frame-pointer -w -I/tk/tk7/src -c -o $(basename $f .cpp).o $f & done; wait
g++ -std=c++20 -g -O1 -w -rdynamic -DSANITIZE -fsanitize=address -o drv $T/drv.cpp video*.o -ldl || exit 1
export LD_LIBRARY_PATH=/out/video/test/stub ASAN_OPTIONS=log_path=$O/report:detect_leaks=0:abort_on_error=0:exitcode=9
nice ./drv a.log --every 2 $T/streams/*.case; nice ./drv b.log --every 3 $T/synth/*.case $T/synth2/*.case
grep -a -h -c "^exit 0" a.log b.log; grep -a -h "^exit [1-9]\|^signal" a.log b.log | sort | uniq -c
ls report* 2>/dev/null | wc -l; cat report* 2>/dev/null | grep -E "ERROR|#[0-4] " | head -40

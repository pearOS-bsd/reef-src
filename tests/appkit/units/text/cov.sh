#!/bin/bash
# cov.sh [ROUNDS SEED [PART]]: in the container: our sources with gcov, the driver, what was not run
rm -rf /tmp/text/cov; mkdir -p /tmp/text/cov; cd /tmp/text/cov
F="-std=c++20 -g -w -I/tmp/text/src -I/tk/tk7/src -I/usr/include/freetype2 -I/usr/include/harfbuzz -include /tk/app/text/stub/extra.h"
for f in text paragraph; do g++ $F --coverage -O0 -fvisibility=hidden -c -o $f.o /tmp/text/src/$f.cpp & done; wait
python3 -B /tk/app/text/tramp.py text.o paragraph.o > tramp.cpp; g++ $F -c -o tramp.o tramp.cpp; g++ $F -O1 -c -o driver.o /tk/app/text/driver.cpp
g++ --coverage -rdynamic -o driver driver.o tramp.o text.o paragraph.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 -ldl -lharfbuzz 2>&1 | grep -v warning | tail -5
ulimit -v 4000000; export FONTCONFIG_FILE=/tk/app/text/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 HOME=/tmp/text/home XDG_RUNTIME_DIR=/tmp/text/home
./driver /o/lib/cutefish-reef/host/libreef-appkit-host.so "$@"
gcov -b text.o paragraph.o > gcov.log 2>&1
for f in text.cpp paragraph.cpp; do echo "== $f"; grep -n "#####" $f.gcov; done

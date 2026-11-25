#!/bin/bash
# run.sh [ROUNDS [SEED]]: in the container: builds the driver with our text.o and paragraph.o (cc.sh first) and runs it.
# The functions of other units come from the original library (tramp.py).  Nothing is drawn on a screen (-ReefHeadless YES).
cd /tmp/text
F="-std=c++20 -g -O1 -w -I/tmp/text/src -I/tk/tk7/src -I/usr/include/freetype2 -I/usr/include/harfbuzz -include /tk/app/text/stub/extra.h"
python3 -B /tk/app/text/tramp.py text.o paragraph.o > tramp.cpp || exit 1
g++ $F -c -o tramp.o tramp.cpp && g++ $F -c -o driver.o /tk/app/text/driver.cpp 2>&1 | grep -E "error" -A3 | head -30
g++ -rdynamic -o driver driver.o tramp.o text.o paragraph.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 -ldl -lharfbuzz 2>&1 | grep -v warning | tail -5
ulimit -v 4000000; export FONTCONFIG_FILE=/tk/app/text/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 HOME=/tmp/text/home XDG_RUNTIME_DIR=/tmp/text/home
mkdir -p $HOME
exec ./driver /o/lib/cutefish-reef/host/libreef-appkit-host.so "$@"

# cc.sh [LO HI]: in the container: compile text.cpp and paragraph.cpp, link, compare (REAL=1: with the headers as they are)
D=/tmp/text; mkdir -p $D/src; cd $D
for f in /w/src/appkit-host/*.h /w/src/appkit-host/text.cpp /w/src/appkit-host/paragraph.cpp; do cmp -s $f src/$(basename $f).in || { cp $f src/$(basename $f).in; cp $f src/$(basename $f); }; done
if [ -n "$REAL" ]; then cp src/fonts.h.in src/fonts.h; else sed -i 's/^    double left;$/    double bounds[4];/; /^    double \(bottom\|right\|top\);$/d' src/fonts.h; fi
F="-std=c++20 -g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -Wdate-time -fPIC -fvisibility=hidden -O3 -DNDEBUG ${W:--w} -I/tk/tk7/src -I/usr/include/freetype2 -I/usr/include/harfbuzz"
for f in text paragraph; do ( g++ $F $X -c -o $f.o src/$f.cpp 2>&1 | grep -E "error|warning" -A3 | head -60 ) & done; wait
g++ -shared -Wl,-z,relro -Wl,--unresolved-symbols=ignore-all -o app.so text.o paragraph.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -lharfbuzz-subset -lharfbuzz -lfontconfig -lfreetype -lz -lEGL -lGLESv2 2>&1 | grep -v warning | tail -5
[ -n "$1" ] && python3 -B /tk/app/range.py /o/lib/cutefish-reef/host/libreef-appkit-host.so $D/app.so $1 $2
true

# in the container: bash /tk/app/fonts/test/cover.sh : which lines of fonts.cpp the groups never reach
D=/tmp/fonts/cov; rm -rf $D; mkdir -p $D/h; cd $D
INC="-I/tk/tk7/src -I/tk/app/stubs -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
LIBS="/root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -lharfbuzz-subset -lharfbuzz -lfontconfig -lfreetype -lz"
g++ -std=c++20 -g -O0 --coverage -fPIC -w $INC -c -o fonts.o /w/src/appkit-host/fonts.cpp && g++ -std=c++20 -fPIC --coverage -shared -w $INC -o ours.so fonts.o /tk/app/fonts/test/table.cpp $LIBS -ldl || exit 1
export LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 APPFONT1=/tmp/fonts/apphack.ttf APPFONT2=/tmp/fonts/app0.ttf GONE=/tmp/fonts/gone
DRV=/tmp/fonts/driver5; PRE=/tmp/fonts/fake5.so
one() { CFFIXED_USER_HOME=$D/h LD_PRELOAD=$PRE timeout 900 $DRV $D/ours.so "$1" > out.$1 2> err.$1; echo "$1 $?"; }
export FONTCONFIG_FILE=/tk/app/fonts/test/fonts.conf
for g in features files match list register reread fallback vary varied data draw mini mini nulls fuzz-shape-1-300 fuzz-draw-1-300 fuzz-match-1-200 fuzz-data-1-100 cache many; do one $g; done
for i in 1 2 3 4 5 6 7 8 9 10 11 12; do FAKE_MTIME=$i one mini > /dev/null; done
FAKE_MTIME=77 one mini; FAKE_MTIME=78 one fuzz-match-1-300; FAKE_MTIME=77 one mini
for t in "Noto Sans Symbols2" "Lato" "Made m-noeight" "Made m-zeroeight" "Nonexistent"; do rm -rf h; mkdir h; FONTCONFIG_FILE=/tk/app/fonts/test/made.conf THEME_FAMILY="$t" one match; done
FONTCONFIG_FILE=/tk/app/fonts/test/gone.conf one gone
FONTCONFIG_FILE=/tk/app/fonts/test/nofonts.conf one match
export FONTCONFIG_FILE=/tk/app/fonts/test/made.conf FONT_ROOTS=/tk/app/fonts/made
for g in names files data list fuzz-draw-5-600 fuzz-shape-5-300 fuzz-match-5-200 fuzz-data-5-100; do one $g; done
truncate -s -5 h/.cache/cutefish-reef/font-answers; one mini
env -u CFFIXED_USER_HOME LD_PRELOAD=$PRE $DRV $D/ours.so mini > /dev/null 2>&1
for n in 100 3000 9000; do head -c $n /dev/urandom > h/.cache/cutefish-reef/font-answers; one mini > /dev/null; done
gcov -o . fonts.o > gcov.out 2>&1; grep -A1 "appkit-host/fonts.cpp" gcov.out

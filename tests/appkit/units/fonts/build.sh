# in the container: bash /tk/app/fonts/test/build.sh : ours.so from the sources, the driver and the preload
set -e
D=/tmp/fonts; mkdir -p $D/conf.d $D/fccache; cd $D
for f in /etc/fonts/conf.d/*.conf; do case $f in *70-no-bitmaps*) ;; *) ln -sf $(readlink -f $f) conf.d/$(basename $f);; esac; done
INC="-I/tk/tk7/src -I/tk/app/stubs -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
OPT="${OPT:--g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -O3 -DNDEBUG}"
LIBS="/root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -lharfbuzz-subset -lharfbuzz -lfontconfig -lfreetype -lz"
[ -n "$ONLYDRIVER" ] || g++ -std=c++20 $OPT -fPIC -fvisibility=hidden -w $INC -shared -o ${OUT:-ours.so} /w/src/appkit-host/fonts.cpp /tk/app/fonts/test/table.cpp $LIBS -ldl
[ -n "$ONLYLIB" ] && exit 0
g++ -std=c++20 -g -O1 -w $INC -o ${DRIVER:-driver} /tk/app/fonts/test/driver.cpp $LIBS -ldl
gcc -shared -fPIC -o ${FAKE:-fake.so} /tk/app/fonts/test/fake.c -ldl

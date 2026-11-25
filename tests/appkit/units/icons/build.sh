# in the container: bash /tk/app/icons/test/build.sh : our library from the sources, the driver and the preload, in /tmp/icons/test
set -e
D=/tmp/icons/test; mkdir -p $D/obj; cd $D
F="-std=c++20 -g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -fPIC -fvisibility=hidden -O3 -DNDEBUG -w -I/tk/tk7/src -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
[ -n "$ASAN" ] && F="-std=c++20 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -fPIC -fvisibility=hidden -w -I/tk/tk7/src -I/w/src/appkit-host" && L="-fsanitize=address,undefined"
O=obj${ASAN:+-asan}; mkdir -p $O
for f in images jpeg2000; do g++ $F -c -o $O/$f.o /w/src/appkit-host/$f.cpp; done
g++ $F -c -o $O/export.o /tk/app/icons/test/export.cpp
mkdir -p t
g++ $L -shared -Wl,-z,defs -o t/ours${ASAN:+-asan}.so $O/images.o $O/jpeg2000.o $O/export.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -ldl
cp -f /o/lib/cutefish-reef/host/libreef-appkit-host.so t/orig.so
[ -e driver ] && [ driver -nt /tk/app/icons/test/driver.cpp ] || g++ -std=c++20 -g -O1 -w -rdynamic -I/tk/tk7/src -I/w/src/appkit-host -o driver /tk/app/icons/test/driver.cpp /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 -ldl -lpthread
g++ -O1 -w -fPIC -shared -o malloc.so /tk/app/icons/test/malloc.cpp -ldl
rm -rf pictures data; cp -r /tk/app/icons/test/files/pictures /tk/app/icons/test/files/data .

# In the container: builds the driver and ours.so into /tmp/desktop/t
D=/tmp/desktop/t${SUF}; mkdir -p $D; cd $D
F="-std=c++20 -g -fPIC -fvisibility=hidden -w -I/tk/tk7/src -I/tk/app/stubs -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz ${SAN:--O2 -O3 -DNDEBUG}"
for u in application desktop now_playing; do g++ $F -c -o $u.o /w/src/appkit-host/$u.cpp & done
g++ $F -c -o export.o /tk/app/desktop/test/export.cpp &
g++ -std=c++20 -g -O1 -w ${SAN} -rdynamic -o driver /tk/app/desktop/test/driver.cpp -ldl &
gcc -shared -fPIC -O1 -o fakehome.so /tk/app/desktop/test/fakehome.c &
wait
g++ -shared ${SAN} -o ours.so application.o desktop.o now_playing.o export.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,--unresolved-symbols=ignore-in-shared-libs -Wl,--no-undefined
ls -la driver ours.so

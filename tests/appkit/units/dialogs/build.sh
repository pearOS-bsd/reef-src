# In the container: builds the driver, the hooks and ours into /tmp/dialogs/t. SAN="-fsanitize=address -O1" for a checked mine.so.
D=/tmp/dialogs/t; mkdir -p $D; cd $D
F="-std=c++20 -g -fPIC -w -I/tk/tk7/src -I/tk/app/stubs -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
if [ -z "$ONLYDRIVER" ]; then
for u in drag dialogs; do g++ $F -fvisibility=hidden ${SAN:--O2 -O3 -DNDEBUG} -c -o $u.o /w/src/appkit-host/$u.cpp || exit 1; done
g++ $F -fvisibility=hidden ${SAN:--O2} -c -o shim.o /tk/app/dialogs/test/shim.cpp || exit 1
fi
g++ -shared ${SAN} -Wl,-z,lazy -o mine.so drag.o dialogs.o shim.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 || exit 1
g++ $F -O1 -shared -o hooks.so /tk/app/dialogs/test/hooks.cpp /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 || exit 1
g++ $F -O1 -rdynamic -o driver /tk/app/dialogs/test/driver.cpp /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 -ldl || exit 1
ls -la mine.so hooks.so driver

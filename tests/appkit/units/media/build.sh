# In the container: builds our test library and the drivers into /tmp/media/test.
set -e
mkdir -p /tmp/media/test /tmp/media/obj; cd /tmp/media/obj
F="-std=c++20 -g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -Wdate-time -fPIC -fvisibility=hidden -O3 -DNDEBUG -w -I/tk/tk7/src -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
for f in sound camera displays jpeg2000 gpu_activity; do
  [ $f.o -nt /w/src/appkit-host/$f.cpp ] && [ $f.o -nt /w/src/appkit-host/$f.h ] || g++ $F -c -o $f.o /w/src/appkit-host/$f.cpp
done
g++ $F -c -o export.o /tk/app/media/test/export.cpp
g++ -shared -Wl,-z,relro -Wl,-z,defs -o /tmp/media/test/ours.so sound.o camera.o displays.o jpeg2000.o gpu_activity.o export.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -lEGL
cd /tmp/media/test
for t in /tk/app/media/test/test_*.cpp; do
  b=$(basename $t .cpp)
  [ $b -nt $t ] && continue
  g++ -std=c++20 -g -O1 -w -I/tk/tk7/src -o $b $t /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -ldl -lpthread -Wl,--unresolved-symbols=ignore-in-shared-libs
done
for s in /tk/app/media/test/fake_*.cpp; do
  b=$(basename $s .cpp)
  [ $b.so -nt $s ] || g++ -std=c++20 -g -O1 -w -fPIC -shared -I/tk/tk7/src -o $b.so $s -ldl
done
mkdir -p stubs/full stubs/nolatency stubs/noread stubs/nowrite stubs/nosndread
for v in full nolatency noread nowrite nosndread; do
  D=-DVARIANT_$v
  g++ -std=c++20 -g -O1 -w -fPIC -shared $D -o stubs/$v/libpulse-simple.so.0 /tk/app/media/test/stub_pulse.cpp
  g++ -std=c++20 -g -O1 -w -fPIC -shared $D -o stubs/$v/libsndfile.so.1 /tk/app/media/test/stub_sndfile.cpp
done
mkdir -p stubs/jp2 stubs/jp2noencoder
g++ -std=c++20 -g -O1 -w -fPIC -shared -o stubs/jp2/libopenjp2.so.7 /tk/app/media/test/stub_openjp2.cpp
g++ -std=c++20 -g -O1 -w -fPIC -shared -DVARIANT_noencoder -o stubs/jp2noencoder/libopenjp2.so.7 /tk/app/media/test/stub_openjp2.cpp

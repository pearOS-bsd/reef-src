# in the container: bash /tk/app/icons/test/cov.sh : which lines of images.cpp the groups reach (after build.sh)
cd /tmp/icons/test; mkdir -p cov/t; cd cov
F="-std=c++20 -g -O0 --coverage -fPIC -fvisibility=hidden -w -I/tk/tk7/src -I/w/src/appkit-host"
g++ $F -c -o images.o /w/src/appkit-host/images.cpp && g++ -std=c++20 -g -O1 -fPIC -fvisibility=hidden -w -I/tk/tk7/src -I/w/src/appkit-host -c -o jpeg2000.o /w/src/appkit-host/jpeg2000.cpp && g++ -std=c++20 -O1 -fPIC -fvisibility=hidden -w -I/tk/tk7/src -I/w/src/appkit-host -c -o export.o /tk/app/icons/test/export.cpp || exit 1
g++ --coverage -shared -o t/ours.so images.o jpeg2000.o export.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -ldl
cp -f ../t/orig.so t/; rm -f *.gcda
export HOME=/tmp/icons/test/home FAKE_HOME=/tmp/icons/test/home LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8
rm -rf t/symbols; ln -s /tk/app/icons/test/files/symbols t/symbols
LD_PRELOAD=/tmp/icons/test/malloc.so timeout 1500 ../driver /tmp/icons/test/cov/t images dmabuf encode decode icons symbols folder
gcov -b -o . images.cpp > gcov.txt 2>&1; grep -A4 "images.cpp'" gcov.txt | head -8
grep -n "#####" images.cpp.gcov

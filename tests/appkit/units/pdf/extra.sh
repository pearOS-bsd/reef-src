# in the container: bash /tk/app/pdf/test/extra.sh : builds the driver for coverage (/tmp/pdf/cov/driver) and with sanitizers (/tmp/pdf/asan/driver)
set -e
INC="-std=c++20 -g -w -I/tk/tk7/src -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
LIBS="/root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -lz -ldl"
mkdir -p /tmp/pdf/cov /tmp/pdf/asan
( cd /tmp/pdf/cov; g++ $INC -O0 --coverage -c -o pdf.o /w/src/appkit-host/pdf.cpp; g++ $INC -O1 -c -o main.o /tk/app/pdf/test/driver.cpp; g++ --coverage -o driver main.o pdf.o $LIBS ) &
( cd /tmp/pdf/asan; S="-fsanitize=address,undefined"; g++ $INC -O1 $S -fno-omit-frame-pointer -c -o pdf.o /w/src/appkit-host/pdf.cpp; g++ $INC -O1 $S -c -o main.o /tk/app/pdf/test/driver.cpp; g++ $S -o driver main.o pdf.o $LIBS ) &
wait

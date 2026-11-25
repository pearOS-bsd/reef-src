# in the container: bash /tk/app/pdf/test/build.sh : the driver with pdf.cpp in it
set -e
D=/tmp/pdf; mkdir -p $D; cd $D
INC="-I/tk/tk7/src -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz"
OPT="${OPT:--g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -O3 -DNDEBUG}"
LIBS="/root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -lz -ldl"
g++ -std=c++20 $OPT -fPIC -w $INC -c -o ${OUT:-driver}-pdf.o ${SRC:-/w/src/appkit-host/pdf.cpp} &
g++ -std=c++20 -g -O1 -w $INC $X -c -o ${OUT:-driver}-main.o /tk/app/pdf/test/driver.cpp &
wait
g++ $X -o ${OUT:-driver} ${OUT:-driver}-main.o ${OUT:-driver}-pdf.o $LIBS
gcc -shared -fPIC -o fixedtime.so /tk/fixedtime.c

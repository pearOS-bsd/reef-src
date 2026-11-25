# In the container: builds the driver into /tmp/dbt (ours linked in).
set -e
D=/tmp/dbt; mkdir -p $D; cd $D
F="-std=c++20 -g -O1 -w -I/tk/tk7/src -I/w/src/appkit-host ${SAN:-}"
for u in menu_bus now_playing status_item; do g++ -std=c++20 -g -O2 -O3 -DNDEBUG -fPIC -fvisibility=hidden -w -I/tk/tk7/src ${SAN:-} -c -o $u.o /w/src/appkit-host/$u.cpp & done; wait
g++ $F -rdynamic -o driver${SUF:-} /tk/app/menu_bus/test/driver.cpp menu_bus.o now_playing.o status_item.o /root2/libcutefish-toolkit-base.so.1 /root2/libcutefish-toolkit.so.1 -ldl -Wl,--unresolved-symbols=ignore-in-shared-libs
ls -la driver${SUF:-}

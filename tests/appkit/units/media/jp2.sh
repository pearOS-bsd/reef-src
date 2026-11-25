# In the container: JPEG 2000 with the stub and with the real library.
cd /tmp/media/test
O=/o/lib/cutefish-reef/host/libreef-appkit-host.so
N=${1:-3000}
for v in jp2 jp2noencoder; do
  LD_LIBRARY_PATH=stubs/$v:/root2 timeout 300 ./test_jpeg2000 $O stub $N > jo.txt 2>&1; a=$?
  LD_LIBRARY_PATH=stubs/$v:/root2 timeout 300 ./test_jpeg2000 ./ours.so stub $N > ju.txt 2>&1; b=$?
  cmp jo.txt ju.txt && echo "$v same: $(wc -l < jo.txt) lines, status $a $b" || diff jo.txt ju.txt | head
done
F=$(ls /tk/app/media/test/images/* 2>/dev/null)
LD_LIBRARY_PATH=/root2 timeout 300 ./test_jpeg2000 $O real $F > jo.txt 2>&1; a=$?
LD_LIBRARY_PATH=/root2 timeout 300 ./test_jpeg2000 ./ours.so real $F > ju.txt 2>&1; b=$?
cmp jo.txt ju.txt && echo "real same: $(wc -l < jo.txt) lines, status $a $b" || diff jo.txt ju.txt | head

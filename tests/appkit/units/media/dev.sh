# In the container: cameras and displays.
cd /tmp/media/test
O=/o/lib/cutefish-reef/host/libreef-appkit-host.so
export LD_LIBRARY_PATH=/root2 LD_PRELOAD="/tmp/media/test/fake_clock.so /tmp/media/test/fake_dev.so"
timeout 300 ./test_camera $O ${1:-60} > co.txt 2>&1; a=$?
timeout 300 ./test_camera ./ours.so ${1:-60} > cu.txt 2>&1; b=$?
cmp co.txt cu.txt && echo "camera same: $(wc -l < co.txt) lines, status $a $b" || diff co.txt cu.txt | head
n=0; bad=0
for m in 0 1; do for s in $(seq 1 ${2:-150}); do
  timeout 20 ./test_displays $O $s $m > do.txt 2>&1; a=$?
  timeout 20 ./test_displays ./ours.so $s $m > du.txt 2>&1; b=$?
  n=$((n+1)); cmp -s do.txt du.txt && [ $a = $b ] || { bad=$((bad+1)); echo "displays differ: seed $s mode $m ($a $b)"; diff do.txt du.txt | head -5; }
  cat do.txt >> dall.txt
done; done
echo "displays: $n runs, $bad different, $(wc -l < dall.txt) lines"; rm -f dall.txt

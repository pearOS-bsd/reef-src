# In the container: the sound functions with the real libsndfile (apt install libsndfile1) and the stub of PulseAudio.
cd /tmp/media/test; mkdir -p stubs/pulse; cp stubs/full/libpulse-simple.so.0 stubs/pulse/
for l in /o/lib/cutefish-reef/host/libreef-appkit-host.so ./ours.so; do
  LD_LIBRARY_PATH=/tmp/media/test/stubs/pulse:/root2 timeout 200 ./test_sound $l real /tk/app/media/test/sounds/* > r.txt 3> r3.txt; echo "status $?" >> r.txt; cat r3.txt >> r.txt; mv r.txt r$(basename $l .so).txt
done
cmp rlibreef-appkit-host.txt rours.txt && echo "real sounds same: $(wc -l < rours.txt) lines, $(grep -c 'way . -> 1' rours.txt) opened, $(grep -c pa_simple_write rours.txt) writes, $(grep status rours.txt)" || diff rlibreef-appkit-host.txt rours.txt | head

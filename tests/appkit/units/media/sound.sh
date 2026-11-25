# In the container: compares the sound functions of the original and ours.
cd /tmp/media/test
O=/o/lib/cutefish-reef/host/libreef-appkit-host.so
run() { # LIB VARIANT CASE
  LD_LIBRARY_PATH=/tmp/media/test/stubs/$2:/root2 LD_PRELOAD=/tmp/media/test/fake_clock.so timeout 120 ./test_sound $1 $3 2>&1; }
for v in full nolatency noread nowrite nosndread none; do
  for c in creation playback control loops failures silence names streams noloop nothing; do
    case $v-$c in full-nothing|nolatency-nothing|noread-nothing) continue;; esac
    case $v-$c in none-names|nosndread-*|nowrite-control|nowrite-loops|none-control|none-loops|none-playback|none-failures|none-silence|nosndread-names) [ $c != noloop ] && [ $c != streams ] && continue;; esac
    run $O $v $c > o.txt 3> o3.txt; a=$?
    run ./ours.so $v $c > u.txt 3> u3.txt; b=$?
    cat o3.txt >> o.txt; cat u3.txt >> u.txt
    if cmp -s o.txt u.txt && [ $a = $b ]; then echo "same $v $c ($(wc -l < o.txt) lines, status $a)"; else echo "DIFFERENT $v $c ($a $b)"; diff o.txt u.txt | head -10; fi
  done
done

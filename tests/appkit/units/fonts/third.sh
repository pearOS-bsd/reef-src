# in the container: bash /tk/app/fonts/test/third.sh : many answers of other libraries beside those of this one; a file that cannot grow
W=/tmp/fonts/third; rm -rf $W; mkdir -p $W; cd $W
export FONTCONFIG_FILE=/tk/app/fonts/test/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8
ORIG=/o/lib/cutefish-reef/host/libreef-appkit-host.so; OURS=${OURS:-/tmp/fonts/ours.so}; D=${DRIVER:-/tmp/fonts/driver5}; P=${PRE:-/tmp/fonts/fake5.so}
run() { rm -rf home; ln -s $3 home; CFFIXED_USER_HOME=$W/home LD_PRELOAD=$P timeout 600 $D $2 ${G:-mini} > out.$1 2> err.$1; echo "exit $?" >> out.$1; }
A=.cache/cutefish-reef/font-answers
same() { if cmp -s $2 $3; then echo "$1: same"; else echo "$1: DIFFERENT $2 $3"; fi; }
mkdir h.orig h.ours
for m in 1 2 3 4 5 6 7 8 9 1 2 3 4 5 1 1; do
  export FAKE_MTIME=$m
  run $m.orig $ORIG h.orig; run $m.ours $OURS h.ours
  same "library $m" out.$m.orig out.$m.ours; same "its answers" h.orig/$A h.ours/$A; echo "size $(stat -c %s h.orig/$A)"
done
ls -la h.orig/.cache/cutefish-reef h.ours/.cache/cutefish-reef | awk '{print $5,$9}'
unset FAKE_MTIME
# no more than some blocks of a file
trap '' XFSZ
for limit in 0 1 2 5 30; do
  rm -rf h.orig h.ours; mkdir h.orig h.ours
  for s in orig ours; do
    L=$ORIG; [ $s = ours ] && L=$OURS; rm -rf home; ln -s h.$s home
    for again in 1 2; do (ulimit -f $limit; FAKE_MTIME=3 CFFIXED_USER_HOME=$W/home LD_PRELOAD=$P timeout 600 $D $L mini 2> /dev/null | cat) | cat > out.l$again.$s; done
    (ulimit -f $limit; for m in 4 5 6; do FAKE_MTIME=$m CFFIXED_USER_HOME=$W/home LD_PRELOAD=$P timeout 600 $D $L mini 2> /dev/null | cat; done) | cat > out.l3.$s
    ls -la h.$s/.cache/cutefish-reef | awk '{print $5,$9}' | grep -v '\.\.$' > ls.$s
  done
  for again in 1 2 3; do same "limit $limit run $again" out.l$again.orig out.l$again.ours; done
  if [ -e h.orig/$A ]; then same "limit $limit answers" h.orig/$A h.ours/$A; fi; same "limit $limit folder" ls.orig ls.ours; cat ls.orig | tr '\n' ' '; echo
done
# many answers of another library between those of this one
rm -rf h.orig h.ours; mkdir h.orig h.ours
FAKE_MTIME=1 run x $ORIG h.orig; echo "size $(stat -c %s h.orig/$A)"; export FAKE_MTIME=2; G=fuzz-match-1-300 run x $ORIG h.orig; unset FAKE_MTIME; echo "size $(stat -c %s h.orig/$A)"; cp -r h.orig/.cache h.ours/; cp -r h.orig h.keep
for s in orig ours; do L=$ORIG; [ $s = ours ] && L=$OURS; export FAKE_MTIME=1; run r.$s $L h.$s; ls -la h.$s/.cache/cutefish-reef | awk '{print $5,$9}' > ls.$s; done; unset FAKE_MTIME
same "written anew" out.r.orig out.r.ours; same "its answers" h.orig/$A h.ours/$A; same "its folder" ls.orig ls.ours; cat ls.orig | tr '\n' ' '; echo
# the same, but a file cannot grow: it is not written anew
for limit in 0 10 40 46 47 100; do
  for s in orig ours; do
    rm -rf h.$s; cp -r h.keep h.$s; L=$ORIG; [ $s = ours ] && L=$OURS; rm -rf home; ln -s h.$s home
    (ulimit -f $limit; FAKE_MTIME=1 CFFIXED_USER_HOME=$W/home LD_PRELOAD=$P timeout 600 $D $L mini 2> /dev/null | cat) | cat > out.m.$s
    ls -la h.$s/.cache/cutefish-reef | awk '{print $5,$9}' > ls.$s
  done
  same "limit $limit not written anew" out.m.orig out.m.ours; same "its answers" h.orig/$A h.ours/$A; same "its folder" ls.orig ls.ours; cat ls.orig | tr '\n' ' '; echo
done

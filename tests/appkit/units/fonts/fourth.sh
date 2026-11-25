# in the container: bash /tk/app/fonts/test/fourth.sh : a fallback whose file went away before it was read
W=/tmp/fonts/fourth; rm -rf $W /tmp/fonts/gone2; mkdir -p $W /tmp/fonts/gone2; cd $W
export FONTCONFIG_FILE=/tk/app/fonts/test/pua.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8
for s in orig ours; do
  L=/o/lib/cutefish-reef/host/libreef-appkit-host.so; [ $s = ours ] && L=${OURS:-/tmp/fonts/ours.so}; rm -rf home h.$s; mkdir h.$s; ln -s h.$s home
  cp /tk/app/fonts/pua-gone/regular.ttf /tmp/fonts/gone2/regular.ttf
  PUA_GONE=/tmp/fonts/gone2/regular.ttf CFFIXED_USER_HOME=$W/home LD_PRELOAD=${PRE:-/tmp/fonts/fake5.so} ${DRIVER:-/tmp/fonts/driver5} $L pua > out.$s 2> err.$s; echo "$s $?"
done
cmp out.orig out.ours && echo same output; cat out.ours; cmp h.orig/.cache/cutefish-reef/font-answers h.ours/.cache/cutefish-reef/font-answers && echo same answers

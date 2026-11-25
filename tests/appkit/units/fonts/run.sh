# in the container: bash /tk/app/fonts/test/run.sh GROUP... : both libraries, each with an empty home; compares the output and the file of answers
W=/tmp/fonts/run/${NAME:-$1}; mkdir -p $W; cd $W
export FONTCONFIG_FILE=${CONF:-/tk/app/fonts/test/fonts.conf} LD_LIBRARY_PATH=/root2 LC_ALL=${LOC:-C.UTF-8}
P=${PRE:-/tmp/fonts/fake.so}
for s in orig ours; do
  L=/o/lib/cutefish-reef/host/libreef-appkit-host.so; [ $s = ours ] && L=${OURS:-/tmp/fonts/ours.so}
  [ -n "$KEEPHOME" ] || rm -rf home.$s; mkdir -p home.$s/h
  # The same path for both.
  rm -rf home; ln -s home.$s home
  SIDE=.$s CFFIXED_USER_HOME=$W/home/h LD_PRELOAD=$P timeout ${T:-600} ${DRIVER:-/tmp/fonts/driver} $L "$@" > out.$s 2> err.$s; echo "$s: exit $? lines $(wc -l < out.$s) err $(wc -l < err.$s)"
done
cmp out.orig out.ours && echo "same output $(md5sum < out.orig | cut -c1-8)" || diff out.orig out.ours | head -${N:-20}
A=home.orig/h/.cache/cutefish-reef/font-answers; B=home.ours/h/.cache/cutefish-reef/font-answers
[ -e $A ] || [ -e $B ] && { cmp $A $B && echo "same answers $(stat -c %s $A)"; }

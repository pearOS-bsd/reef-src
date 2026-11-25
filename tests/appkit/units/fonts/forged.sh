# in the container: the answers are believed: every byte of the file changed in turn (in steps), both outputs the same, how many of them not those of the first run
G=mini; W=/tmp/fonts/forged; rm -rf $W; mkdir -p $W; cd $W
export FONTCONFIG_FILE=/tk/app/fonts/test/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8
ORIG=/o/lib/cutefish-reef/host/libreef-appkit-host.so; OURS=${OURS:-/tmp/fonts/ours.so}; D=${DRIVER:-/tmp/fonts/driver2}; P=${PRE:-/tmp/fonts/fake2.so}
run() { rm -rf home; ln -s $3 home; CFFIXED_USER_HOME=$W/home LD_PRELOAD=$P timeout 600 $D $2 $G > out.$1 2> err.$1; echo "exit $?" >> out.$1; }
A=.cache/cutefish-reef/font-answers
mkdir h.orig h.ours; run 1orig $ORIG h.orig; cp h.orig/$A a1; mkdir -p h.ours/.cache/cutefish-reef
S=$(stat -c %s a1); same=0; changed=0; n=0; crash=0
for at in $(seq ${FROM:-0} ${STEP:-97} ${TO:-$S}); do
  python3 -c "
import sys
d=bytearray(open('a1','rb').read()); i=$at
if i<len(d): d[i]=(d[i]+1)&255 if $at%3 else d[i]^0x80
open('h.orig/$A','wb').write(d)"
  cp h.orig/$A h.ours/$A
  run 2orig $ORIG h.orig; run 2ours $OURS h.ours
  n=$((n+1)); if cmp -s out.2orig out.2ours && cmp -s h.orig/$A h.ours/$A; then same=$((same+1)); else echo "DIFFERENT at $at"; fi
  cmp -s out.1orig out.2orig || changed=$((changed+1)); grep -q "^exit 0" out.2orig || crash=$((crash+1))
done
echo "runs $n same $same, output not that of the first run $changed, crashed (both) $crash"

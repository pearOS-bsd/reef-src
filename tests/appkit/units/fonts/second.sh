# in the container: bash /tk/app/fonts/test/second.sh GROUP : the file of answers after the first run: again, of the other library, damaged, of another library
G=${1:-mini}; W=/tmp/fonts/second/$G; rm -rf $W; mkdir -p $W; cd $W
export FONTCONFIG_FILE=${CONF:-/tk/app/fonts/test/fonts.conf} LD_LIBRARY_PATH=/root2 LC_ALL=${LOC:-C.UTF-8}
ORIG=/o/lib/cutefish-reef/host/libreef-appkit-host.so; OURS=${OURS:-/tmp/fonts/ours.so}; D=${DRIVER:-/tmp/fonts/driver}; P=${PRE:-/tmp/fonts/fake.so}
# run NAME LIB HOME
run() { rm -rf home; ln -s $3 home; CFFIXED_USER_HOME=$W/home LD_PRELOAD=$P timeout 600 $D $2 $G > out.$1 2> err.$1; echo "exit $?" >> out.$1; }
A=.cache/cutefish-reef/font-answers
same() { if cmp -s $2 $3; then echo "$1: same"; else echo "$1: DIFFERENT $2 $3"; fi; }
mkdir h.orig h.ours
run 1orig $ORIG h.orig; run 1ours $OURS h.ours; same "first output" out.1orig out.1ours; same "first answers" h.orig/$A h.ours/$A
ls -laR h.orig | awk '{print $1,$2,$5,$9}' > ls.1; ls -laR h.ours | awk '{print $1,$2,$5,$9}' | sed 's/h.ours/h.orig/' > ls.2; same "folders" ls.1 ls.2
cp h.orig/$A a1
run 2orig $ORIG h.orig; run 2ours $OURS h.ours; same "second output" out.2orig out.2ours; same "second answers" h.orig/$A h.ours/$A; same "second as first" h.orig/$A a1
diff out.1orig out.2orig > /dev/null && echo "the same with and without answers" || echo "DIFFERENT with answers: $(diff out.1orig out.2orig | grep -c '^[<>]') lines"
# each with the answers of the other
run 3orig $ORIG h.ours; run 3ours $OURS h.orig; same "crossed output" out.3orig out.3ours; same "crossed answers" h.orig/$A h.ours/$A; same "crossed as first" h.orig/$A a1
# another library (its answers stay)
export FAKE_MTIME=5
run 4orig $ORIG h.orig; run 4ours $OURS h.ours; same "another output" out.4orig out.4ours; same "another answers" h.orig/$A h.ours/$A; echo "sizes $(stat -c %s a1) $(stat -c %s h.orig/$A)"
for i in 6 7 8 9 10 11 12 13 14 15 16; do export FAKE_MTIME=$i; run 5orig $ORIG h.orig; run 5ours $OURS h.ours; same "another $i output" out.5orig out.5ours; same "another $i answers" h.orig/$A h.ours/$A; echo "sizes $(stat -c %s h.orig/$A)"; done
unset FAKE_MTIME
# cut and damaged
for cut in 0 1 7 8 9 100 1000 $(( $(stat -c %s a1) - 1 )) $(( $(stat -c %s a1) / 2 )); do
  head -c $cut a1 > h.orig/$A; cp h.orig/$A h.ours/$A
  run 6orig $ORIG h.orig; run 6ours $OURS h.ours; same "cut $cut output" out.6orig out.6ours; same "cut $cut answers" h.orig/$A h.ours/$A
done
for seed in $(seq 1 ${SEEDS:-40}); do
  python3 - a1 h.orig/$A $seed <<'P'
import sys,random
d=bytearray(open(sys.argv[1],'rb').read()); r=random.Random(int(sys.argv[3]))
for i in range(r.choice([1,1,2,5,20])):
    k=r.randrange(5); p=r.randrange(len(d))
    if k==0: d[p]=r.randrange(256)
    elif k==1: d[p]^=1<<r.randrange(8)
    elif k==2: del d[p:p+r.randrange(1,20)]
    elif k==3: d[p:p]=bytes(r.randrange(256) for _ in range(r.randrange(1,20)))
    else: d+=bytes(r.randrange(256) for _ in range(r.randrange(1,40)))
open(sys.argv[2],'wb').write(d)
P
  cp h.orig/$A h.ours/$A
  run 7orig $ORIG h.orig; run 7ours $OURS h.ours; same "damaged $seed output" out.7orig out.7ours; same "damaged $seed answers" h.orig/$A h.ours/$A
done
# homes that are none
for what in none file ro cachefile; do
  rm -rf h.orig h.ours; mkdir h.orig h.ours
  for s in orig ours; do
    case $what in none) rm -rf h.$s;; file) rm -rf h.$s; touch h.$s;; ro) mkdir h.$s/.cache; chmod 555 h.$s/.cache;; cachefile) touch h.$s/.cache;; esac
  done
  run 8orig $ORIG h.orig; run 8ours $OURS h.ours; same "home $what output" out.8orig out.8ours
  ls -laR h.orig 2>&1 | awk '{print $1,$2,$5,$9}' > ls.1; ls -laR h.ours 2>&1 | awk '{print $1,$2,$5,$9}' | sed 's/h.ours/h.orig/' > ls.2; same "home $what folders" ls.1 ls.2
done
# no home in the environment, or not a whole path: that of the user
R=$(getent passwd $(id -u) | cut -d: -f6)/.cache/cutefish-reef; rm -rf keep; [ -d $R ] && cp -r $R keep
for h in "" home ./home "unset"; do
  for s in orig ours; do
    L=$ORIG; [ $s = ours ] && L=$OURS; rm -rf $R
    if [ "$h" = unset ]; then env -u CFFIXED_USER_HOME LD_PRELOAD=$P $D $L $G > out.9$s 2> err.9$s; else CFFIXED_USER_HOME="$h" LD_PRELOAD=$P $D $L $G > out.9$s 2> err.9$s; fi
    cp $R/font-answers answers.$s 2> /dev/null
  done
  same "home '$h' output" out.9orig out.9ours; same "home '$h' answers" answers.orig answers.ours; same "home '$h' as first" answers.orig a1; rm -f answers.*
done
rm -rf $R; [ -d keep ] && cp -r keep $R

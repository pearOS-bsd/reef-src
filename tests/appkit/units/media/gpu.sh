# In the container: the log of GPU work.
cd /tmp/media/test
O=/o/lib/cutefish-reef/host/libreef-appkit-host.so
n=0; bad=0; rm -f gall.txt
# one NAME SEED HOMEDIR(what the log is under) PREPARE-SIZE ENV...
one() {
  local name=$1 seed=$2 home=$3 size=$4; shift 4
  for l in o u; do
    rm -rf /tmp/media/gpu; mkdir -p "$home" 2>/dev/null
    [ $size -ge 0 ] && mkdir -p $home/.cache/cutefish-reef && head -c $size /dev/zero | tr '\0' 'z' > $home/.cache/cutefish-reef/gpu-activity.log
    [ $size = -2 ] && rmdir "$home"
    L=$O; [ $l = u ] && L=./ours.so
    env "$@" LD_LIBRARY_PATH=/root2 LD_PRELOAD=/tmp/media/test/fake_gpu.so timeout 60 ./test_gpu $L $seed $home/.cache/cutefish-reef/gpu-activity.log > g$l.txt 2>&1 3> g3$l.txt; echo "status $?" >> g$l.txt
    cat g3$l.txt >> g$l.txt
    find /tmp/media/gpu 2>/dev/null | sort >> g$l.txt
  done
  n=$((n+1)); cat go.txt >> gall.txt
  cmp -s go.txt gu.txt || { bad=$((bad+1)); echo "different: $name seed $seed"; diff go.txt gu.txt | head -6; }
}
H=/tmp/media/gpu/home
for s in $(seq 1 ${1:-40}); do one normal $s $H -1 CFFIXED_USER_HOME=$H TZ=$(echo "UTC Europe/Bucharest America/St_Johns Asia/Kathmandu XYZ-13:30" | cut -d' ' -f$((s%5+1))); done
for s in 1 2 3; do
  one relative $s $H -1 CFFIXED_USER_HOME=home FAKE_USER=$H
  one user $s $H -1 FAKE_USER=$H
  one nouser $s $H -1 X=1
  one nulldir $s $H -1 FAKE_USER=null
  one emptydir $s $H -1 FAKE_USER=
  one emptyhome $s $H -1 CFFIXED_USER_HOME= FAKE_USER=$H
  one nofolder $s $H -2 CFFIXED_USER_HOME=$H
  one rootfolder $s /tmp/media/gpu/a-home-folder-with-a-long-name-for-the-strings -1 CFFIXED_USER_HOME=/tmp/media/gpu/a-home-folder-with-a-long-name-for-the-strings
  one size262143 $s $H 262143 CFFIXED_USER_HOME=$H
  one size262144 $s $H 262144 CFFIXED_USER_HOME=$H
  one size262145 $s $H 262145 CFFIXED_USER_HOME=$H
  one size262100 $s $H 262100 CFFIXED_USER_HOME=$H
  one slash $s /tmp/media/gpu/h -1 CFFIXED_USER_HOME=/tmp/media/gpu/h/
done
one missing 1 $H -1 CFFIXED_USER_HOME=$H FAKE_MISSING=eglCreateSyncKHR
echo "gpu: $n runs, $bad different, $(wc -l < gall.txt) lines, $(grep -c '^LOG' gall.txt) log lines"

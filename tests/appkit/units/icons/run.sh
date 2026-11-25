# in the container: bash /tk/app/icons/test/run.sh GROUP... (after build.sh): both libraries in /tmp/icons/test/t with SYMBOLS (default: the package's)
cd /tmp/icons/test
T=${NAME:-t}; mkdir -p $T
[ $T = t ] || { cp -f t/orig.so t/ours.so $T/; }
rm -rf $T/symbols; ln -s ${SYMBOLS:-/o/lib/cutefish-reef/host/symbols} $T/symbols
export HOME=/tmp/icons/test/home FAKE_HOME=/tmp/icons/test/home XDG_DATA_HOME=/tmp/icons/test/data XDG_DATA_DIRS=/tmp/icons/test/nothing XDG_RUNTIME_DIR=/tmp/icons/test/home XDG_CONFIG_HOME=/tmp/icons/test/home LD_LIBRARY_PATH=/root2 LC_ALL=${LOC:-C.UTF-8}
mkdir -p $HOME; rm -rf $HOME/.icons; cp -r data/icons $HOME/.icons
[ -n "$NOPRELOAD" ] || P=/tmp/icons/test/malloc.so
LD_PRELOAD=$P timeout ${TIME:-900} ./driver ${DIR:-/tmp/icons/test/$T} "$@"; echo "exit $?"

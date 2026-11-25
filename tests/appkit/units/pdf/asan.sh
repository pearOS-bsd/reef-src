# in the container: bash /tk/app/pdf/test/asan.sh GROUP... : the driver with sanitizers (build: see the comment at the end of run.sh)
cd /tmp/pdf
export FONTCONFIG_FILE=/tmp/pdf/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 CFFIXED_USER_HOME=/tmp/pdf/home HOME=/tmp/pdf/home OUTPUT=/out/pdf/asan
export ASAN_OPTIONS=detect_leaks=${LEAKS:-0}:halt_on_error=1:abort_on_error=0:detect_odr_violation=0 UBSAN_OPTIONS=print_stacktrace=1
timeout ${T:-900} /tmp/pdf/asan/driver "$@"

# in the container: bash /tk/app/pdf/test/run.sh GROUP... : the groups of driver.cpp; what differs is written to /out/pdf (KEEP=1: all documents)
# bash /tk/app/pdf/test/build.sh first. All groups: document structure paths images gradients groups shadows glyphs matched text more fuzz=1-2000
mkdir -p /tmp/pdf/conf.d /tmp/pdf/fccache /tmp/pdf/home; cd /tmp/pdf
for f in /etc/fonts/conf.d/*.conf; do case $f in *70-no-bitmaps*) ;; *) ln -sf $(readlink -f $f) conf.d/$(basename $f);; esac; done
sed 's,/tmp/fonts/,/tmp/pdf/,g' /tk/app/fonts/test/fonts.conf > fonts.conf
export FONTCONFIG_FILE=/tmp/pdf/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 CFFIXED_USER_HOME=/tmp/pdf/home HOME=/tmp/pdf/home
LD_PRELOAD=/tmp/pdf/fixedtime.so timeout ${T:-900} ${DRIVER:-./driver} "$@"

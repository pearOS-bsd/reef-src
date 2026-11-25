#!/bin/bash
# go.sh ROUNDS SEED [PART]: in the container: runs the driver that run.sh built
cd /tmp/text
ulimit -v 4000000
export FONTCONFIG_FILE=/tk/app/text/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 HOME=/tmp/text/home XDG_RUNTIME_DIR=/tmp/text/home
exec ./${DRIVER:-driver} /o/lib/cutefish-reef/host/libreef-appkit-host.so "$@"

#!/bin/bash
# go2.sh CONF ROUNDS SEED [PART]: as go.sh with another fontconfig file (fonts2.conf: Monaco and Courier are installed)
cd /tmp/text
ulimit -v 4000000
export FONTCONFIG_FILE=/tk/app/text/$1 LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 HOME=/tmp/text/home2 XDG_RUNTIME_DIR=/tmp/text/home2
mkdir -p $HOME; shift
exec ./${DRIVER:-driver} /o/lib/cutefish-reef/host/libreef-appkit-host.so "$@"

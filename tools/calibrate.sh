#!/bin/sh
# In the container: the known-answer test of the toolchain.
# usage: calibrate.sh TOOLKIT_CHECKOUT_111537f ORIGINAL_libcutefish-toolkit-base.so THIS_REPO
set -e
H="-g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -Wdate-time"
mkdir -p /tmp/cal; ln -sfn "$1/src" /tmp/cal/src; cp "$3/docker/calibration.cmake" /tmp/cal/CMakeLists.txt
cmake -S /tmp/cal -B /tmp/cb -G Ninja -DCMAKE_BUILD_TYPE=None -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_C_FLAGS="$H" -DCMAKE_CXX_FLAGS="$H" >/dev/null
ninja -C /tmp/cb cutefish-toolkit-base >/dev/null
python3 "$3/tools/compare.py" --min 100 "$2" "$(find /tmp/cb -name libcutefish-toolkit-base.so.1.0)"

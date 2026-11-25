#!/bin/bash
# In the container: builds the stubs and the two drivers into /out/video/test.
S=/tk/app/video/test; O=/out/video/test; B=/out/video/build
mkdir -p $O; cd $O
stub() { d=$1; shift; mkdir -p $d; gcc -shared -fPIC -w -O1 "$@" -o $d/libva.so.2 -Wl,-soname,libva.so.2 $S/va_stub.c; gcc -shared -fPIC -w -O1 -DDRM -o $d/libva-drm.so.2 -Wl,-soname,libva-drm.so.2 $S/va_stub.c; }
stub stub
stub stub_noexport -DNO_export
stub stub_noderive -DNO_derive
stub stub_nocallbacks -DNO_callbacks
F="-std=c++20 -g -O1 -w -rdynamic"
[ drv_orig -nt $S/drv.cpp ] || g++ $F -DORIGINAL -o drv_orig $S/drv.cpp -ldl
bash /tk/app/video/vc.sh
g++ $F -o drv_ours $S/drv.cpp $B/video*.o -ldl
ls -la drv_orig drv_ours

#!/bin/bash
# run_fonts.sh [ROUNDS SEED PART]: as run.sh, but our side uses our fonts unit (objects of appc.sh in /tmp/textall); PART is Text, Paragraph or FontMetrics
mkdir -p /tmp/text/wf; cd /tmp/text/wf
A=/tmp/textall
F="-std=c++20 -g -O1 -w -I/w/src/appkit-host -I/tk/tk7/src -I/usr/include/freetype2 -I/usr/include/harfbuzz -include /tk/app/text/stub/extra.h"
sed 's/addr={/addr={"x":0,/' /tk/app/text/tramp.py > tramp.py
python3 -B - <<'PY' > tramp.cpp
import subprocess
addr={'PdfWriter::setTransform':0xd01e0,'PdfWriter::transform':0xd0210,'PdfWriter::drawText':0xedfe0,'PdfWriter::drawGlyphs':0xedf80,'systemFontFamily':0x32100,'copyString':0x3bf70}
A='/tmp/textall/'
und=set(); de=set()
for o in ('text.o','paragraph.o','fonts.o','paths.o'):
    for l in subprocess.run(['nm',A+o],capture_output=True,text=True).stdout.split('\n'):
        p=l.split()
        if len(p)>=2 and p[-2]=='U': und.add(p[-1])
        elif len(p)==3: de.add(p[2])
print('#include <cstdint>\nextern "C" { uintptr_t originalBase; }\nasm(R"(\n.text')
for s in sorted(und-de):
    d=subprocess.run(['c++filt',s],capture_output=True,text=True).stdout.strip()
    if not d.startswith('cutefish::reef::'): continue
    n=d[len('cutefish::reef::'):].split('(')[0].replace('[abi:cxx11]','')
    a=addr.get(n)
    if a is None: print(f'/* {n} */\n.globl {s}\n.hidden {s}\n{s}:\n  ud2'); continue
    print(f'.globl {s}\n.hidden {s}\n.type {s},@function\n{s}:\n  mov originalBase(%rip),%r11\n  add $0x{a:x},%r11\n  jmp *%r11')
print(')");')
PY
grep "^/\*" tramp.cpp
g++ $F -c -o tramp.o tramp.cpp && g++ $F -c -o driver.o /tk/app/text/driver.cpp 2>&1 | grep -E "error" -A3 | head -30
g++ -rdynamic -o driver driver.o tramp.o $A/text.o $A/paragraph.o $A/fonts.o $A/paths.o /root2/libcutefish-toolkit.so.1 /root2/libcutefish-toolkit-base.so.1 -Wl,-rpath-link,/root2 -ldl -lharfbuzz-subset -lharfbuzz -lfontconfig -lfreetype -lz 2>&1 | grep -v warning | tail -5
ulimit -v 4000000
export FONTCONFIG_FILE=/tk/app/text/fonts.conf LD_LIBRARY_PATH=/root2 LC_ALL=C.UTF-8 HOME=/tmp/text/home3 XDG_RUNTIME_DIR=/tmp/text/home3
mkdir -p $HOME
exec ./driver /o/lib/cutefish-reef/host/libreef-appkit-host.so "$@"

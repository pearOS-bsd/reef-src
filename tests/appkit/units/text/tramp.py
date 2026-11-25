# tramp.py: jumps to the functions of other units in the original library, for the undefined functions of text.o and paragraph.o
import subprocess,sys
addr={'faceMatch':0x89840,'faceRetain':0x7f260,'faceRelease':0x82e70,'faceMetrics':0x7f270,'faceGlyphs':0x7f2e0,'faceShape':0x803a0,'faceFallback':0x8e5c0,
'parseFeatures':0x81a70,'drawGlyphs':0x90650,'PdfWriter::setTransform':0xd01e0,'PdfWriter::transform':0xd0210,'PdfWriter::drawText':0xedfe0,
'PdfWriter::drawGlyphs':0xedf80,'systemFontFamily':0x32100,'copyString':0x3bf70}
und=set(); de=set()
for o in sys.argv[1:]:
    for l in subprocess.run(['nm',o],capture_output=True,text=True).stdout.split('\n'):
        p=l.split()
        if len(p)>=2 and p[-2]=='U': und.add(p[-1])
        elif len(p)==3: de.add(p[2])
out=[]
for s in sorted(und-de):
    d=subprocess.run(['c++filt',s],capture_output=True,text=True).stdout.strip()
    if not d.startswith('cutefish::reef::'): continue
    n=d[len('cutefish::reef::'):].split('(')[0].replace('[abi:cxx11]','')
    out.append((s,addr[n]))
print('#include <cstdint>\nextern "C" { uintptr_t originalBase; }\nasm(R"(\n.text')
for i,(s,a) in enumerate(out):
    print(f'.globl {s}\n.hidden {s}\n.type {s},@function\n{s}:\n  mov originalBase(%rip),%r11\n  add $0x{a:x},%r11\n  jmp *%r11')
print(')");')

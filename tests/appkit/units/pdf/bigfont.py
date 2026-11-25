# bigfont.py FILE : a Type 1 font whose A has 600 lines, more than the first buffer for outlines holds
import sys,math
def enc(data,r,skip=4):
    out=bytearray()
    for b in b'\0'*skip+data:
        c=b^(r>>8); out.append(c); r=((c+r)*52845+22719)&0xffff
    return bytes(out)
def num(v):
    if -107<=v<=107: return bytes([v+139])
    if 108<=v<=1131: v-=108; return bytes([247+(v>>8),v&255])
    if -1131<=v<=-108: v=-v-108; return bytes([251+(v>>8),v&255])
    raise ValueError
def cs(*p): return b''.join(num(x) if isinstance(x,int) else bytes(x) for x in p)
HSBW,RLINE,RMOVE,CLOSE,END=[13],[5],[21],[9],[14]
def glyph(n):
    pts=[(round(500+400*math.cos(2*math.pi*i/n)),round(500+(400-60*(i%2))*math.sin(2*math.pi*i/n))) for i in range(n)]
    s=cs(0,1000,HSBW); x=y=0
    for i,(a,b) in enumerate(pts):
        s+=cs(a-x,b-y,RLINE if i else RMOVE); x,y=a,b
    return s+cs(CLOSE,END)
g={'.notdef':cs(0,500,HSBW,END),'A':glyph(600),'B':glyph(20),'C':glyph(255),'D':glyph(257)}
private=b'dup /Private 8 dict dup begin\n/RD {string currentfile exch readstring pop} executeonly def\n/ND {noaccess def} executeonly def\n/NP {noaccess put} executeonly def\n/BlueValues [] def\n/MinFeature {16 16} def\n/password 5839 def\n/Subrs 0 array\nND\n2 index /CharStrings %d dict dup begin\n'%len(g)
for k,v in g.items():
    e=enc(v,4330); private+=b'/%s %d RD '%(k.encode(),len(e))+e+b' ND\n'
private+=b'end\nend\nreadonly put\nnoaccess put\ndup /FontName get exch definefont pop\nmark currentfile closefile\n'
head='''%!PS-AdobeFont-1.0: BigOutline 001.000
11 dict begin
/FontInfo 9 dict dup begin
/version (001.000) readonly def
/FullName (BigOutline) readonly def
/FamilyName (BigOutline) readonly def
/Weight (Regular) readonly def
/ItalicAngle 0 def
/isFixedPitch false def
/UnderlinePosition -100 def
/UnderlineThickness 50 def
end readonly def
/FontName /BigOutline def
/PaintType 0 def
/FontType 1 def
/FontMatrix [0.001 0 0 0.001 0 0] readonly def
/Encoding 256 array
0 1 255 {1 index exch /.notdef put} for
dup 65 /A put
dup 66 /B put
dup 67 /C put
dup 68 /D put
readonly def
/FontBBox {0 0 1000 1000} readonly def
currentdict end
currentfile eexec
'''
h=enc(private,55665).hex()
open(sys.argv[1],'w').write(head+'\n'.join(h[i:i+64] for i in range(0,len(h),64))+'\n'+('0'*64+'\n')*8+'cleartomark\n')

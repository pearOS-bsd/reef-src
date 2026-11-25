# pdfcmp.py A B: the objects of two PDF files in which they differ, streams inflated
import sys,re,zlib
def objs(p):
    d=open(p,'rb').read(); o={}
    for m in re.finditer(rb'(\d+) 0 obj\n(.*?)\nendobj',d,re.S):
        b=m.group(2); s=re.search(rb'^(.*?)stream\n(.*)\nendstream$',b,re.S)
        if s:
            t=s.group(2)
            if b'FlateDecode' in s.group(1):
                try: t=zlib.decompress(t)
                except Exception as e: t=b'?'+str(e).encode()
            b=s.group(1)+b'stream '+t
        o[int(m.group(1))]=b
    return o
a,b=objs(sys.argv[1]),objs(sys.argv[2])
print(len(a),len(b))
for k in sorted(set(a)|set(b)):
    x,y=a.get(k,b''),b.get(k,b'')
    if x!=y:
        i=next((i for i in range(min(len(x),len(y))) if x[i]!=y[i]),0)
        print(k,len(x),len(y),i); print('  A',x[max(0,i-80):i+120]); print('  B',y[max(0,i-80):i+120])

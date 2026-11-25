# rename.py IN OUT FAMILY: a copy of a TrueType font with another family name
import sys,struct
src,out,fam=sys.argv[1:4]
d=open(src,'rb').read()
n=struct.unpack('>H',d[4:6])[0]
tabs={}
for i in range(n):
    t,c,o,l=struct.unpack('>4sIII',d[12+16*i:28+16*i]); tabs[t]=d[o:o+l]
nm=tabs[b'name']; fmt,cnt,so=struct.unpack('>HHH',nm[:6])
recs=[]; blob=b''
for i in range(cnt):
    p,e,l,i_,ln,off=struct.unpack('>HHHHHH',nm[6+12*i:18+12*i])
    s=nm[so+off:so+off+ln]
    if i_ in (1,16): s=fam.encode('utf-16-be') if p in (0,3) else fam.encode()
    if i_ in (4,): s=fam.encode('utf-16-be') if p in (0,3) else fam.encode()
    if i_==6: s=(fam.replace(' ','')+'-Regular').encode('utf-16-be') if p in (0,3) else (fam.replace(' ','')+'-Regular').encode()
    recs.append((p,e,l,i_,len(s),len(blob))); blob+=s
tabs[b'name']=struct.pack('>HHH',0,cnt,6+12*cnt)+b''.join(struct.pack('>HHHHHH',*r) for r in recs)+blob
pad=lambda b:b+b'\0'*(-len(b)%4)
cs=lambda b:sum(struct.unpack('>%dI'%(len(pad(b))//4),pad(b)))&0xffffffff
h=d[:12]; off=12+16*n; dirs=b''; body=b''
for t in sorted(tabs):
    b=tabs[t]; dirs+=struct.pack('>4sIII',t,cs(b),off+len(body),len(b)); body+=pad(b)
open(out,'wb').write(h+dirs+body)

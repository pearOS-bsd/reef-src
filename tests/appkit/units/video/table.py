# What the cases of a log did.
import sys
cur=None; rows={}
for line in open(sys.argv[1], errors='replace'):
    if line.startswith('==== '):
        cur=line[5:].strip(); rows[cur]=dict(dec='',pics=0,frames=0,ok=0,fail=0,none=0,end='')
    elif cur:
        r=rows[cur]
        if line.startswith(' decoder') or line.startswith(' none '): r['dec']=line.strip()
        elif 'vaEndPicture' in line: r['pics']+=1
        elif line.startswith(' 1 frame'): r['frames']+=1; r['ok']+=1
        elif line.startswith(' 1 '): r['ok']+=1; r['none']+=1
        elif line.startswith(' 0 '): r['fail']+=1
        elif line.startswith('exit') or line.startswith('signal'): r['end']=line.strip()
for k,r in rows.items(): print(f"{k:34s} {r['dec']:22s} pics {r['pics']:4d} frames {r['frames']:4d} ok {r['ok']:4d} (no frame {r['none']}) failed {r['fail']} {r['end']}")

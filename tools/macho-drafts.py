#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Draft sources (tools/macho-gen-objc.py) for every file of a list, built and compared: one line per file with the function match.
usage: macho-drafts.py LIST_FILE    (in the image; LIST_FILE: paths under the root, one per line; drafts go to /tmp/mb/batch)"""
import subprocess,os,sys,importlib.util,json
R='/o/lib/cutefish-reef/'
def load(n,f):
    s=importlib.util.spec_from_file_location(n,'/w/tools/'+f); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
os.environ['MACHO_SRC']='/tmp/mb/batch'
bm=load('bm','build-macho.py'); cm=load('cm','compare-macho.py')
res=[]
for f in open(sys.argv[1]).read().split():
    d=f.split('/Versions/')[0] if '.framework/' in f else f.rsplit('.dylib',1)[0]
    folder='/tmp/mb/batch/'+d
    subprocess.run(['rm','-rf',folder])
    r=subprocess.run(['python3','/w/tools/macho-gen-objc.py',R+f,folder,f],capture_output=True,text=True)
    gen=r.stderr.strip().splitlines()[-1] if r.stderr.strip() else r.stdout[-100:]
    if r.returncode: res.append((d,'GEN FAIL',r.stderr.strip().splitlines()[-1][:100])); continue
    try:
        bm.build(R,'/tmp/mb/bout',d)
    except SystemExit:
        res.append((d,'BUILD FAIL',gen)); continue
    A=cm.Image(R+f); B=cm.Image('/tmp/mb/bout/'+f)
    found,same=cm.compare(A,B)
    tot=sum(A.funcs.values()); ok=sum(A.funcs[x] for x in same)
    res.append((d,'%d/%d fn %.1f%%'%(len(same),len(A.funcs),100*ok/max(tot,1)),gen))
for r in res: print(*r,sep=' | ')

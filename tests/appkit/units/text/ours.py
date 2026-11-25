import sys,subprocess
sys.path.insert(0,'/w/tools'); sys.dont_write_bytecode=True
import compare
U='/tmp/text/app.so'; name=sys.argv[1]
im=compare.Image(U)
syms=[k for k in im.symbols if 'cold' not in k and im.symbols[k] in im.funcs]
dem=subprocess.run(['c++filt','-p'],input='\n'.join(syms),capture_output=True,text=True).stdout.split('\n')
c=[k for k,d in zip(syms,dem) if d==name or d=='cutefish::reef::'+name] or [k for k,d in zip(syms,dem) if name in d]
a=im.symbols[c[0]]; n=im.funcs[a]
o=subprocess.run(['objdump','-d','--no-show-raw-insn','--start-address=%d'%a,'--stop-address=%d'%(a+n),U],capture_output=True,text=True).stdout
o=subprocess.run(['c++filt'],input=o,capture_output=True,text=True).stdout
for l in o.split('\n'):
    if ':\t' in l and 'nop' not in l and 'endbr' not in l: print(l.replace('cutefish::','').strip()[:130])

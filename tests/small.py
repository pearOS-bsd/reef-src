#!/usr/bin/env python3
"""Differential tests of login and reef-authhelper.  usage: small.py ORIGINAL_USR BUILD_DIR   (in a container, as root)"""
import os, socket, struct, subprocess, sys, tempfile, time
orig, ours = sys.argv[1:3]
bad = 0
def check(name, f, a, b):
    global bad
    x, y = f(a), f(b)
    print('ok  ' if x == y else 'FAIL', name, x if x == y else (x, y)); bad += x != y

def login(args, user=None, env=None):
    def f(p):
        pre = (lambda: os.setuid(user)) if user else None
        r = subprocess.run(['login', *args], executable=p, capture_output=True, preexec_fn=pre, env={'TERM': 'xterm', 'A': 'b', **(env or {})}, stdin=subprocess.DEVNULL, timeout=20)
        return r.returncode, r.stdout, r.stderr
    return f
L = [orig + '/lib/cutefish-reef/usr/bin/login', ours + '/login']
for args, user in [(['-z'], None), (['-f', 'nosuchuser'], None), (['-f', 'root', 'env'], None), (['-fp', 'root', 'env'], None),
                   (['-fl', 'root', '/bin/pwd'], None), (['-f', 'root', '/bin/pwd'], None), (['-fq', 'root', '/nonexistent'], None),
                   (['-f', 'nobody', 'id'], None), (['-f', 'root', 'id'], 65534), (['-f'], 65534), (['-f', 'daemon', 'env'], None)]:
    check('login %s uid %s' % (args, user), login(args, user), *L)

def auth(cookie=b'c' * 16, sent=None, count=None, args=(b'/bin/echo', b'hi'), size=None, pass_fd=True, cut=None, probe=None):
    def f(p):
        d = tempfile.mkdtemp(); path = d + '/s'
        a, b = socket.socketpair()
        loader = d + '/loader'
        open(loader, 'w').write('#!/bin/sh\necho loader "$@" | sed s#%s#D#\n' % d); os.chmod(loader, 0o755)
        prog = d + '/macho'; open(prog, 'wb').write(b'\xcf\xfa\xed\xfe' + b'\0' * 64)
        r = subprocess.Popen([p, loader, path, str(os.getuid())], stdin=b)
        b.close(); a.sendall(cookie); a.settimeout(5)
        if len(cookie) < 16: a.shutdown(socket.SHUT_WR); return 'exit', r.wait(5)
        ready = a.recv(1); st = os.stat(path)
        s = socket.socket(socket.AF_UNIX); s.settimeout(5); s.connect(path)
        rd, wr = os.pipe()
        data = b'\0'.join(x.replace(b'MACHO', prog.encode()) for x in args) + b'\0'
        msg = (sent or cookie) + struct.pack('<II', len(args) if count is None else count, len(data) if size is None else size) + data
        if cut: msg = msg[:cut]
        if pass_fd: socket.send_fds(s, [msg], [wr])
        else: s.sendall(msg)
        os.close(wr); s.shutdown(socket.SHUT_WR)
        try: reply = s.recv(64)
        except OSError as e: reply = type(e).__name__
        out = os.read(rd, 4096) if pass_fd else b''
        a.close()
        return ready, oct(st.st_mode), reply, out, r.wait(5), os.path.exists(path)
    return f
A = [orig + '/bin/reef-authhelper', ours + '/reef-authhelper']
check('usage', lambda p: subprocess.run([p, 'a']).returncode, *A)
check('long path', lambda p: subprocess.run([p, 'x', '/tmp/' + 'a' * 120, '0'], input=b'c' * 16).returncode, *A)
for name, kw in {'run': {}, 'wrong cookie': dict(sent=b'x' * 16), 'short cookie': dict(cookie=b'c'), 'no descriptor': dict(pass_fd=False),
                 'missing program': dict(args=(b'/nonexistent',)), 'count 0': dict(count=0), 'count 4097': dict(count=4097),
                 'count above text': dict(count=9), 'size above limit': dict(size=(1 << 20) + 1), 'cut in cookie': dict(cut=7),
                 'cut in text': dict(cut=26), 'Mach-O through the loader': dict(args=(b'MACHO', b'one', b'two'))}.items():
    check('authhelper ' + name, auth(**kw), *A)
sys.exit(bool(bad))

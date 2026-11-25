# client.py LIBRARY DRIVER LOG : runs the scenarios with the library on a private bus and writes a log without what differs from run to run
import os, re, select, shutil, subprocess, sys, tempfile, time
import dbus, dbus.service, dbus.bus, dbus.lowlevel
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

library, driver, logname = sys.argv[1:4]
driver = os.path.abspath(driver)
home = '/tmp/desktop/home'          # the same for both, so that paths are
log = open(logname, 'w')
def say(*a):
    log.write(' '.join(str(x) for x in a) + '\n'); log.flush()

ctx = GLib.MainContext.default()
def pump(seconds):
    end = time.time() + seconds
    while time.time() < end:
        if not ctx.iteration(False): time.sleep(0.002)

def until(condition, seconds=10):
    end = time.time() + seconds
    while not condition() and time.time() < end:
        if not ctx.iteration(False): time.sleep(0.002)

class Driver:
    def __init__(self, name, address, *extra, env=None):
        self.name = name
        e = dict(os.environ, LD_LIBRARY_PATH='/root2', HOME=home, TEST_HOME=home, LD_PRELOAD=os.path.dirname(driver) + '/fakehome.so',
                 XDG_RUNTIME_DIR=home + '/run', XDG_CURRENT_DESKTOP='Cutefish', LANG='C', LC_ALL='C', PATH=home + '/bin:/usr/bin:/bin')
        e.pop('DBUS_SESSION_BUS_ADDRESS', None)
        if address: e['DBUS_SESSION_BUS_ADDRESS'] = address
        e.update(env or {})
        self.errors = open('/tmp/desktop/driver.err', 'a')
        self.p = subprocess.Popen([driver, library, *extra], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=self.errors, env=e, cwd=home)
        self.data = b''
        self.dead = False
    def line(self, timeout=20):
        end = time.time() + timeout
        while b'\n' not in self.data:
            if time.time() > end: return None
            ctx.iteration(False)
            r, _, _ = select.select([self.p.stdout], [], [], 0.002)
            if r:
                d = os.read(self.p.stdout.fileno(), 65536)
                if not d:
                    self.dead = True
                    return None
                self.data += d
        l, self.data = self.data.split(b'\n', 1)
        return l.decode('utf-8', 'replace')
    def do(self, *fields):
        self.p.stdin.write('\t'.join(str(f) for f in fields).encode() + b'\n'); self.p.stdin.flush()
        if fields[0] == 'sleep': return
        while True:
            l = self.line()
            if l is None:
                say(self.name, '>', '|'.join(map(str, fields)), '->', 'ENDED %s' % self.p.wait() if self.dead else 'NO ANSWER')
                return
            say(self.name, '>', '|'.join(map(str, fields)), '->', l) if l.split(' ')[0] in (fields[0], 'unknown') else say(self.name, 'event', l)
            if l.split(' ')[0] in (fields[0], 'unknown'): return l
    def events(self, seconds=0.3):
        end = time.time() + seconds
        while time.time() < end:
            l = self.line(max(0.01, end - time.time()))
            if l is not None: say(self.name, 'event', l)
            elif self.dead: return
    def end(self):
        if not self.dead: self.do('end')
        self.p.stdin.close(); self.p.wait()

class Bus:
    def __init__(self, title):
        say('====', title)
        self.daemon = subprocess.Popen(['dbus-daemon', '--session', '--nofork', '--print-address=1'], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        self.address = self.daemon.stdout.readline().decode().strip()
        self.out = open('/tmp/desktop/monitor.txt', 'w')
        self.monitor = subprocess.Popen(['dbus-monitor', '--address', self.address], stdout=self.out, stderr=subprocess.DEVNULL)
        time.sleep(0.4)
    def connection(self):
        c = dbus.bus.BusConnection(self.address)
        c.set_exit_on_disconnect(False)
        return c
    def close(self):
        time.sleep(0.3)
        self.monitor.terminate(); self.monitor.wait()
        self.daemon.terminate(); self.daemon.wait()
        self.out.close()
        normalise('/tmp/desktop/monitor.txt')

# The messages the monitor saw, with the connections renamed in the order they came and the serials by order of each connection.
def normalise(name):
    names = {}
    serials = {}
    def conn(m):
        return names.setdefault(m, 'conn%d' % len(names))
    lines = []
    for l in open(name, errors='replace'):
        l = l.rstrip('\n')
        l = re.sub(r'time=\d+\.\d+ ', '', l)
        l = re.sub(r':1\.\d+', lambda m: conn(m[0]), l)
        l = re.sub(r'instance\d+', 'instancePID', l)
        l = re.sub(r'unix process id|uint32 \d{4,}', lambda m: 'uint32 N' if m[0].startswith('uint32') else m[0], l)
        l = re.sub(r'(reply_)?serial=\d+', lambda m: (m[1] or '') + 'serial=N', l)
        lines.append(l)
    # The list of the names of the bus comes in any order.
    i = 0
    while i < len(lines):
        j = i
        while j < len(lines) and lines[j].startswith('      string '): j += 1
        if j > i and '      string "org.freedesktop.DBus"' in lines[i:j]: lines[i:j] = sorted(lines[i:j])
        i = max(j, i + 1)
    for l in lines: say('bus', l)

class FileManager(dbus.service.Object):
    calls = 0
    @dbus.service.method('org.freedesktop.FileManager1', in_signature='ass', out_signature='', message_keyword='message')
    def ShowItems(self, uris, startup, message): FileManager.calls += 1; say('ShowItems', [str(u) for u in uris], repr(str(startup)), 'noreply', message.get_no_reply())
    @dbus.service.method('org.freedesktop.FileManager1', in_signature='ass', out_signature='', message_keyword='message')
    def ShowFolders(self, uris, startup, message): FileManager.calls += 1; say('ShowFolders', [str(u) for u in uris], repr(str(startup)), 'noreply', message.get_no_reply())

class Instance(dbus.service.Object):
    failures = 0
    @dbus.service.method('org.cutefish.Application', in_signature='ass', out_signature='')
    def Launch(self, items, token):
        say('Launch', [str(i) for i in items], repr(str(token)), 'failures left', self.failures)
        if self.failures > 0:
            self.failures -= 1
            raise dbus.DBusException('Not yet', name='org.cutefish.Error.NotYet')

def call(c, name, path, interface, method, *args, signature=None, timeout=3):
    box = []
    m = dbus.lowlevel.MethodCallMessage(name, path, interface, method)
    if args or signature: m.append(signature=signature, *args)
    c.send_message_with_reply(m, lambda r: box.append(r), timeout)
    end = time.time() + timeout + 1
    while not box and time.time() < end:
        if not ctx.iteration(False): time.sleep(0.002)
    r = box[0] if box else None
    if r is None: t = 'none'
    elif r.get_type() == 3: t = 'error %s %s' % (r.get_error_name(), [str(x) for x in r.get_args_list()])
    else: t = 'reply %s %s' % (r.get_signature(), show(r.get_args_list()))
    say('call', re.sub(r'instance\d+', 'instancePID', name), method, args, '->', t)
    return r

def show(v):
    if isinstance(v, dict): return '{' + ', '.join('%s: %s' % (show(k), show(v[k])) for k in sorted(v, key=str)) + '}'
    if isinstance(v, (list, tuple)): return '[' + ', '.join(show(x) for x in v) + ']'
    if isinstance(v, dbus.Boolean): return str(bool(v))
    if isinstance(v, str): return repr(str(v))
    return str(v)

PROPS = 'org.freedesktop.DBus.Properties'
APP = 'org.cutefish.Application'
PATH = '/org/cutefish/Application'

DBusGMainLoop(set_as_default=True)
sys.path.insert(0, os.path.dirname(__file__))
import files
files.make(home)

ONLY = sys.argv[4:]
def wanted(n): return not ONLY or n in ONLY

# ---- what needs no bus
if wanted('files'):
    say('==== files')
    d = Driver('d', None)
    # before the application is there
    d.do('token', 'early'); d.do('token', '~'); d.do('dock', 0); d.do('dock', 0); d.do('dock', 5); d.do('claim', 'org.a', 1, 0); d.do('show', 1, 1, '/tmp')
    for p in files.paths(home):
        for size in (300, 1, 0, -1, 5, 10, 11, 12): d.do('type', p, size)
        d.do('type', p, 300, 'null')
    for t in files.types:
        for generic in (0, 1, -7):
            for size in (200, 0, -5, 1, 4): d.do('icon', t, generic, size)
            d.do('icon', t, generic, 100, 'null')
    d.do('init', 'org.test.Driver', 'driver', '-ReefHeadless', 'YES')
    d.do('default', 'text/plain', 300)          # starts the reading of the applications
    pump(1); d.events(1)
    for t in files.types:
        for size in (300, 0, -1, 1, 7): d.do('default', t, size)
        d.do('default', t, 300, 'null')
    # headless without a bus
    d.do('claim', 'org.a', 1, 0); d.do('show', 1, 1, '/tmp'); d.do('dock', 1); d.do('playing'); d.do('playing', 'App', 'T', 'A', 'B', '~', 10, 1, 1, 1, 1023, 7); d.do('playing')
    d.end()

# ---- starting desktop entries
if wanted('open'):
    say('==== open')
    d = Driver('d', None)
    for e, arguments in files.launches(home):
        out = home + '/started'
        if os.path.exists(out): os.remove(out)
        r = d.do('open', e, len(arguments), *arguments)
        if r and r.endswith(' 0'): until(lambda: os.path.exists(out))
        else: pump(0.05)
        say('started', repr(open(out).read()) if os.path.exists(out) else 'nothing')
    d.do('open', home + '/.local/share/applications/one.desktop', 0)
    d.do('open', home + '/.local/share/applications/one.desktop', -3, 'x')
    d.do('open', home + '/.local/share/applications/one.desktop', 1, '~')
    d.do('open', '~', 0)
    d.end()

INIT = ['-ReefHeadless', 'YES', '-ReefHeadlessSessionBus', 'YES']

# ---- the file manager
if wanted('show'):
    b = Bus('show')
    d = Driver('d', b.address)
    d.do('init', 'org.test.Driver', 'driver', *INIT)
    d.do('show', 1, 1, '/tmp/without service')
    pump(0.3)
    c = b.connection()
    n = dbus.service.BusName('org.freedesktop.FileManager1', c)
    o = FileManager(n, '/org/freedesktop/FileManager1')
    def show_(*fields):
        before = FileManager.calls
        r = d.do(*fields)
        if r and r.endswith(' 1'): until(lambda: FileManager.calls > before)
        else: pump(0.05)
    for selected in (0, 1, 2, -1):
        show_('show', selected, 1, '/tmp/a b')
        show_('show', selected, 3, '/tmp/a#b%c?d', '/ünï/日本', 'relative/path')
        show_('show', selected, 5, '~', '', '/x', '~', '/y/')
        show_('show', selected, 2, '~', '')
        show_('show', selected, 2, '/one', '/two', '/three')
        show_('show', selected, 0, '/one')
        show_('show', selected, -2)
    show_('show', 1, 40, *['/long/%s/%d' % ('x' * 40, i) for i in range(40)])
    d.end(); b.close()

# ---- the instance
if wanted('claim'):
    for identifier in files.identifiers:
        b = Bus('claim %r' % identifier)
        d = Driver('d', b.address)
        d.do('init', 'org.test.Driver', 'driver', *INIT)
        d.do('claim', identifier, 0, 0)
        c = b.connection()
        pump(0.1)
        say('names', sorted(str(x) for x in c.list_names() if not str(x).startswith(':')))
        d.end(); b.close()

if wanted('object'):
    for nocallbacks in (False, True):
        b = Bus('object %s' % nocallbacks)
        d = Driver('d', b.address, *(['nocallbacks'] if nocallbacks else []))
        c = b.connection()
        seen = []
        c.add_signal_receiver(lambda *a, **k: say('signal', k['member'], k['path'], show(a)), path=PATH, member_keyword='member', path_keyword='path')
        d.do('init', 'org.test.Driver', 'driver', *INIT)
        d.do('dock', 0); d.events(0.2)
        d.do('claim', 'com.example.First', 0, 0)
        name = 'org.cutefish.Application.com.example.First'
        call(c, name, PATH, 'org.freedesktop.DBus.Introspectable', 'Introspect')
        call(c, name, PATH, PROPS, 'GetAll', APP)
        call(c, name, PATH, PROPS, 'Get', APP, 'AppId')
        call(c, name, PATH, PROPS, 'Get', APP, 'ShowsInDock')
        call(c, name, PATH, PROPS, 'Get', APP, 'Nothing')
        call(c, name, PATH, PROPS, 'Set', APP, 'AppId', dbus.String('x', variant_level=1), signature='ssv')
        for v in (0, 1, 1, 0, 17, 0, -1):
            d.do('dock', v)
            call(c, name, PATH, 'org.freedesktop.DBus.Peer', 'Ping')
        call(c, name, PATH, PROPS, 'Get', APP, 'ShowsInDock')
        call(c, name, PATH, APP, 'Launch', dbus.Array([], signature='s'), '', signature='ass'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array(['/a b', 'http://x/', ''], signature='s'), 'token-1', signature='ass'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array(['x' * 300] * 20, signature='s'), 'y' * 100, signature='ass'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', signature=None); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array(['a'], signature='s'), signature='as'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array([1], signature='i'), 't', signature='ais'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array(['a'], signature='s'), 5, signature='asi'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array(['a'], signature='s'), 't', 5, signature='assi'); d.events(0.1)
        call(c, name, PATH, APP, 'Launch', dbus.Array(['a'], signature='o') if False else 'a', 't', signature='ss'); d.events(0.1)
        call(c, name, PATH, APP, 'Quit'); d.events(0.1)
        call(c, name, PATH, APP, 'Quit', 'extra', signature='s'); d.events(0.1)
        call(c, name, PATH, APP, 'Nothing'); d.events(0.1)
        # again, and as another one: the object stays what it was
        d.do('claim', 'com.example.First', 1, 2, 'a', 'b'); d.do('claim', 'com.example.First', 0, 0); d.do('claim', 'com.example.Second', 1, 1, 'a')
        say('names', sorted(str(x) for x in c.list_names() if not str(x).startswith(':')))
        call(c, 'org.cutefish.Application.com.example.Second', PATH, PROPS, 'GetAll', APP)
        d.do('claim', '~', 1, 0); d.do('claim', '', 1, 0)
        d.end(); b.close()

if wanted('forward'):
    b = Bus('forward')
    first = Driver('first', b.address)
    first.do('init', 'org.test.Driver', 'driver', *INIT)
    first.do('claim', 'com.example.App', 1, 1, 'mine')
    # The other process serves the call while this one waits: give it time with a pump of its own events.
    second = Driver('second', b.address)
    second.do('init', 'org.test.Driver', 'driver', *INIT)
    second.do('token', '~'); second.do('token', 'activation-token-of-some-length'); second.do('token', '~')
    second.do('claim', 'com.example.App', 0, 1, 'not this'); first.events(0.2)
    second.do('claim', 'com.example.App', 1, 4, '/a', '~', 'http://b/', ''); first.events(0.2)
    second.do('claim', 'com.example.App', 1, 1, 'again'); first.events(0.2)          # the token was taken
    second.do('token', 'other'); second.do('claim', 'com.example.App', 1, 0); first.events(0.2)
    second.do('token', 'a'); second.do('token', ''); second.do('claim', 'com.example.App', 2, -1, 'x'); first.events(0.2)
    second.do('claim', 'com.example.App', 1, 1, 'x' * 500); first.events(0.2)
    first.end()
    second.do('claim', 'com.example.App', 1, 1, 'now mine')
    second.end(); b.close()

if wanted('owner'):
    for failures in (0, 1, 2, 4, 5, 9):
        b = Bus('owner %d' % failures)
        c = b.connection()
        n = dbus.service.BusName('org.cutefish.Application.com.example.App', c)
        o = Instance(n, PATH); o.failures = failures
        d = Driver('d', b.address)
        d.do('init', 'org.test.Driver', 'driver', *INIT)
        d.do('token', 'tok')
        d.do('claim', 'com.example.App', 0, 1, 'a')
        d.do('claim', 'com.example.App', 1, 2, 'a', 'b')
        o.failures = 0
        d.do('claim', 'com.example.App', 1, 1, 'c')
        d.end(); b.close()
    b = Bus('owner without object')
    c = b.connection()
    n = dbus.service.BusName('org.cutefish.Application.com.example.App', c)
    d = Driver('d', b.address)
    d.do('init', 'org.test.Driver', 'driver', *INIT)
    d.do('claim', 'com.example.App', 1, 2, 'a', 'b')
    d.end(); b.close()

if wanted('playing'):
    b = Bus('playing')
    c = b.connection()
    d = Driver('d', b.address)
    d.do('init', 'org.test.Driver', 'driver', *INIT)
    def names():
        pump(0.1)
        say('names', sorted(re.sub(r'instance\d+', 'instancePID', str(x)) for x in c.list_names() if not str(x).startswith(':')))
        return [str(x) for x in c.list_names() if 'MediaPlayer2' in str(x)]
    d.do('playing'); names()
    d.do('playing', 'App', 'T', 'A', 'B', '~', 10, 1, 0, 0, 1023, 7); names()
    d.do('playing', 'App', 'T', 'A', 'B', '/art.png', 10, 1, 0, 2, 1023, 7); m = names()
    if m:
        for i in ('org.mpris.MediaPlayer2', 'org.mpris.MediaPlayer2.Player'):
            call(c, m[0], '/org/mpris/MediaPlayer2', PROPS, 'GetAll', i)
        for x in ('Play', 'Pause', 'PlayPause', 'Stop', 'Next', 'Previous'): call(c, m[0], '/org/mpris/MediaPlayer2', 'org.mpris.MediaPlayer2.Player', x); d.events(0.1)
        call(c, m[0], '/org/mpris/MediaPlayer2', 'org.mpris.MediaPlayer2', 'Raise'); d.events(0.1)
    d.do('playing', 'App', 'T2', 'A', 'B', '~', 10, 1, 0, 2, 3, 8); names()
    d.do('playing'); names()
    d.do('playing', 'Other', 'T', 'A', 'B', '~', 10, 1, 0, 3, 1, 7); m = names()
    if m: call(c, m[0], '/org/mpris/MediaPlayer2', PROPS, 'GetAll', 'org.mpris.MediaPlayer2')
    d.do('playing', '~', '~', '~', '~', '~', 0, 0, 0, 0, 0, 0); names()
    d.do('playing')
    d.end(); b.close()
if wanted('busgone'):
    b = Bus('busgone')
    d = Driver('d', b.address)
    d.do('init', '~', 'driver', *INIT)
    d.do('claim', 'com.example.App', 0, 0)
    d.do('playing', 'App', 'T', 'A', 'B', '~', 10, 1, 0, 1, 1023, 7)
    d.do('playing', '~', '~', '~', '~', '~', 10, 1, 0, 1, 1023, 7)
    pump(0.2)
    b.close()
    pump(0.2)
    d.do('show', 1, 1, '/tmp'); d.do('dock', 0); d.do('dock', 1)
    d.do('claim', 'com.example.App', 1, 1, 'x'); d.do('claim', 'com.example.Other', 1, 1, 'x'); d.do('claim', 'com.example.Other', 0, 1, 'x')
    d.do('playing', 'App', 'T', 'A', 'B', '~', 10, 1, 0, 2, 1023, 8); d.do('playing')
    d.end()
say('==== all')

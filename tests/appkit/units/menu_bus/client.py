# client.py LOG DRIVER [LIBRARY] : in the container. Starts a private bus and the driver, runs every scenario, writes the log.
import sys, os, subprocess, time, random, re, signal, struct, tempfile
import gi
gi.require_version('Gio', '2.0')
from gi.repository import Gio, GLib
logname, driver = sys.argv[1], sys.argv[2:]
log = open(logname, 'w')
dbus = subprocess.Popen(['dbus-daemon', '--session', '--nofork', '--nopidfile', '--print-address=1'], stdout=subprocess.PIPE, text=True)
address = dbus.stdout.readline().strip()
os.environ['DBUS_SESSION_BUS_ADDRESS'] = address
env = dict(os.environ, LD_LIBRARY_PATH='/root2')
monitor = subprocess.Popen(['dbus-monitor', '--address', address], stdout=open(logname + '.monitor', 'w'))
time.sleep(.4)
prog = subprocess.Popen(driver, stdout=subprocess.PIPE, text=True, env=env)
ready = prog.stdout.readline()
assert ready.startswith('ready'), ready
unique = ready.split()[1]

def connect():
    c = Gio.DBusConnection.new_for_address_sync(address, Gio.DBusConnectionFlags.AUTHENTICATION_CLIENT | Gio.DBusConnectionFlags.MESSAGE_BUS_CONNECTION, None, None)
    return c
bus = connect()
names = {unique: ':DRIVER', bus.get_unique_name(): ':CLIENT'}
def clean(s):
    s = s.replace('instance%d' % prog.pid, 'instancePID')
    for k, v in names.items(): s = re.sub(re.escape(k) + r'\b', v, s)
    return s
def out(*a):
    log.write(clean(' '.join(str(x) for x in a)) + '\n'); log.flush()
def show(v): return v.print_(True) if v is not None else '-'
def flush():
    ctx = GLib.MainContext.default()
    while ctx.iteration(False): pass
def call(dest, path, iface, member, sig=None, *args, quiet=False, flags=Gio.DBusCallFlags.NO_AUTO_START):
    try:
        r = bus.call_sync(dest, path, iface, member, GLib.Variant(sig, args) if sig else None, None, flags, 5000, None)
        t = show(r)
    except GLib.Error as e:
        r = None; t = 'ERROR %s: %s' % (Gio.DBusError.get_remote_error(e) or e.code, e.message)
    if not quiet: out('call', path, iface + '.' + member, sig, args, '->', t)
    return r
def drv(member, sig=None, *args):
    r = bus.call_sync(unique, '/driver', 'test.Driver', member, GLib.Variant(sig, args) if sig else None, None, Gio.DBusCallFlags.NONE, 5000, None)
    return r.unpack()[0] if r and r.n_children() else None
def sync(what=None):
    for _ in range(3): drv('Ping')
    flush()
    for e in drv('TakeEvents'): out('  event', e)
def do(member, sig=None, *args):
    r = drv(member, sig, *args)
    out('driver', member, args, '->', r)
    sync()
    return r
def on_signal(c, sender, path, iface, member, params):
    out('  signal', sender, path, iface, member, show(params))
bus.signal_subscribe(unique, None, None, None, None, Gio.DBusSignalFlags.NONE, on_signal)
bus.signal_subscribe('org.freedesktop.DBus', 'org.freedesktop.DBus', 'NameOwnerChanged', None, None, Gio.DBusSignalFlags.NONE,
                     lambda c, s, p, i, m, v: out('  owner', clean(show(v))))
def props(dest, path, iface):
    call(dest, path, 'org.freedesktop.DBus.Properties', 'GetAll', '(s)', iface)
def get(dest, path, iface, name): call(dest, path, 'org.freedesktop.DBus.Properties', 'Get', '(ss)', iface, name)
def put(dest, path, iface, name, v): call(dest, path, 'org.freedesktop.DBus.Properties', 'Set', '(ssv)', iface, name, v)
def introspect(dest, path): 
    r = call(dest, path, 'org.freedesktop.DBus.Introspectable', 'Introspect', quiet=True)
    out('introspect', path, '\n' + (r.unpack()[0] if r else 'none'))
random.seed(7)
exec(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'scenario.py')).read())
try: drv('Quit')
except GLib.Error as e: pass
code = prog.wait(5)
out('exit', code)
time.sleep(.3)
monitor.terminate(); dbus.terminate()

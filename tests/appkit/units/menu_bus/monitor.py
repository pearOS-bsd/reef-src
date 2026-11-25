# monitor.py FILE: the messages of the driver in a log of dbus-monitor, without serials, times and unique names
import sys, re
text = open(sys.argv[1], errors='replace').read()
# the messages
msgs = re.split(r'\n(?=(?:method call|method return|signal|error) time=)', '\n' + text)
m = re.search(r'method call .*sender=(:\S+) -> .*member=RequestName\n\s+string "test.Driver"', text)
drv = m[1]
pid = None
seen = {}
def name(x):
    return seen.setdefault(x, ':N%d' % len(seen))
out = []
for g in msgs:
    g = g.strip('\n')
    head = g.split('\n', 1)[0]
    if 'time=' not in head: continue
    if 'sender=%s ' % drv not in head + ' ' : continue
    g = re.sub(r'time=\S+ ', '', g); g = re.sub(r'reply_serial=\d+', 'reply', g); g = re.sub(r' serial=\d+', '', g)
    g = g.replace(drv, ':DRIVER'); g = re.sub(r':1\.\d+', ':X', g); g = re.sub(r'instance\d+', 'instancePID', g); g = re.sub(r'/tmp/dbus-\w+', '/tmp/dbus', g)
    g = re.sub(r'guid=\w+|unix:.*', '', g)
    out.append(g)
print('\n'.join(out))

M = 'com.canonical.dbusmenu'
P = 'org.freedesktop.DBus.Properties'
def V(s, v): return GLib.Variant(s, v)

# ---------------------------------------------------------------- menus
out('=== menu')
SEP, DIS, HID, CHK, MIX = 1, 2, 4, 8, 16
items = [
    ('File', 0x1000, 0, 0, '', 0),
    ('_New', 0x1001, 1, 0, 'n', 2),
    ('Open__Recent', 0x1002, 1, 0, '', 0),
    ('a_b_c', 0x1003, 2, CHK, 'x', 3),
    ('second', 0x1004, 2, MIX, 'F5', 0),
    ('', 0x1005, 1, SEP, 'q', 2),
    ('Quit äö⌘ with a long long long title', 0x1006, 1, DIS, 'q', 2),
    ('Edit', 0x1007, 0, 0, '', 0),
    ('hidden', 0x1008, 1, HID, 'Delete', 15),
    ('both', 0, 1, CHK | MIX | DIS | HID, 'a', 1),
    ('View', 0x100a, 0, 0, 'v', 4),
    ('Help', 0x100b, 0, SEP | DIS, 'h', 8),
]
def every(path, ids, depth=True):
    call(unique, path, M, 'GetLayout', '(iias)', 0, -1, [])
    for i in ids:
        call(unique, path, M, 'GetLayout', '(iias)', i, -1, [])
        if depth:
            call(unique, path, M, 'GetLayout', '(iias)', i, 0, [])
            call(unique, path, M, 'GetLayout', '(iias)', i, 1, ['label', 'nothing'])
    call(unique, path, M, 'GetGroupProperties', '(aias)', list(ids), [])
    call(unique, path, M, 'GetGroupProperties', '(aias)', [], ['label'])
    call(unique, path, M, 'GetGroupProperties', '(aias)', [3, 3, -1, 1000000], ['label'])
    for i in ids:
        for n in ('type', 'label', 'enabled', 'visible', 'toggle-type', 'toggle-state', 'shortcut', 'children-display', 'icon-name', ''):
            call(unique, path, M, 'GetProperty', '(is)', i, n)
def inner(m, ids):
    for i in ids: out('inner', i, drv('MenuHasChildren', '(ii)', m, i), drv('MenuChildren', '(ii)', m, i) if (i in valid) else '')
valid = range(0, 13)
m = do('MenuNew', '(bb)', True, True)
do('MenuSetItems', '(ia(sxiusu))', m, items[:3])
call(unique, '/Menu1', M, 'GetLayout', '(iias)', 0, -1, [])
do('MenuSetPath', '(is)', m, '/Menu1')
introspect(unique, '/Menu1'); introspect(unique, '/')
props(unique, '/Menu1', M)
for n in ('Version', 'TextDirection', 'Status', 'IconThemePath', 'Nothing'): get(unique, '/Menu1', M, n)
put(unique, '/Menu1', M, 'Status', V('s', 'x')); put(unique, '/Menu1', M, 'Version', V('u', 4))
every('/Menu1', range(-1, 5))
do('MenuSetPath', '(is)', m, '/Menu2')
call(unique, '/Menu2', M, 'GetLayout', '(iias)', 0, -1, [])
do('MenuSetItems', '(ia(sxiusu))', m, items)
every('/Menu1', range(0, 18))
valid = [0] + list(range(4, 16)); inner(m, range(-2, 20))
# wrong arguments
for member, sig, args in (('GetLayout', None, ()), ('GetLayout', '(ii)', (0, -1)), ('GetLayout', '(uias)', (0, 1, [])), ('GetLayout', '(iiau)', (0, 1, [])), ('GetLayout', '(iiasi)', (0, 1, [], 1)),
                          ('GetGroupProperties', '(auas)', ([1], [])), ('GetGroupProperties', None, ()), ('GetGroupProperties', '(ai)', ([1],)), ('GetProperty', '(i)', (1,)), ('GetProperty', '(si)', ('a', 1)), ('GetProperty', None, ()),
                          ('Event', '(isv)', (1, 'clicked', V('i', 0))), ('Event', None, ()), ('AboutToShow', '(u)', (1,)), ('AboutToShow', None, ()), ('AboutToShow', '(ii)', (0, 1)), ('Nothing', None, ())):
    call(unique, '/Menu1', M, member, sig, *args)
def event(path, i, kind): call(unique, path, M, 'Event', '(isvu)', i, kind, V('s', 'data'), 5); sync()
for i in range(-1, 18): event('/Menu1', i, 'clicked')
for k in ('hovered', 'opened', 'closed', '', 'Clicked', 'clicked ', 'clickeD'): event('/Menu1', 5, k)
for i in range(-1, 18): call(unique, '/Menu1', M, 'AboutToShow', '(i)', i); sync()
# the same shape
def change(i, **k):
    global items
    t = list(items[i])
    for n, v in k.items(): t['title item depth flags key modifiers'.split().index(n)] = v
    items = items[:i] + [tuple(t)] + items[i + 1:]
do('MenuSetItems', '(ia(sxiusu))', m, items)
change(0, title='Datei'); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(1, flags=DIS); change(2, flags=HID | CHK); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(3, key='y'); change(4, modifiers=7); change(5, item=0x9999); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(5, title='in a separator', flags=SEP | DIS | 0x100); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(6, title='Quit äö⌘ with a long long long titlf'); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(7, title=''); change(8, key=''); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(0, title='A', flags=0xfffffffe); do('MenuSetItems', '(ia(sxiusu))', m, items)
every('/Menu1', range(3, 17), False)
# another shape
change(1, flags=SEP); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(1, flags=0); do('MenuSetItems', '(ia(sxiusu))', m, items)
change(4, depth=1); do('MenuSetItems', '(ia(sxiusu))', m, items)
every('/Menu1', range(38, 54), False)
do('MenuSetItems', '(ia(sxiusu))', m, items[:-1])
do('MenuSetItems', '(ia(sxiusu))', m, items[:-1])
do('MenuSetItems', '(ia(sxiusu))', m, [])
every('/Menu1', [0, 1, 60], False)
do('MenuSetItems', '(ia(sxiusu))', m, [])
do('MenuSetItems', '(ia(sxiusu))', m, items)
# odd depths
odd = [('a', 1, 2, 0, '', 0), ('b', 2, 0, 0, '', 0), ('c', 3, 3, 0, '', 0), ('d', 4, 4, 0, '', 0), ('e', 5, 1, 0, '', 0), ('f', 6, -1, 0, '', 0), ('g', 7, 0, 0, '', 0), ('h', 8, 1, 0, '', 0)]
do('MenuSetItems', '(ia(sxiusu))', m, odd)
b = [x for x in range(0, 120) if drv('MenuChildren', '(ii)', m, 0)][0:1] and drv('MenuChildren', '(ii)', m, 0)
out('top', b)
lay = call(unique, '/Menu1', M, 'GetLayout', '(iias)', 0, -1, [])
first = None
for i in range(1, 400):
    r = call(unique, '/Menu1', M, 'GetGroupProperties', '(aias)', [i], [], quiet=True)
    if r.unpack()[0] and first is None: first = i
out('first id', first)
valid = [0] + list(range(first, first + 8)); inner(m, range(first - 1, first + 10))
every('/Menu1', range(first, first + 8))
# a deep one and a long one
deep = [('level %d' % i, 0x2000 + i, i, 0, '', 0) for i in range(12)]
do('MenuSetItems', '(ia(sxiusu))', m, deep)
call(unique, '/Menu1', M, 'GetLayout', '(iias)', 0, -1, []); call(unique, '/Menu1', M, 'GetLayout', '(iias)', 0, 5, [])
for trial in range(40):
    n = random.randrange(0, 30); d = 0; l = []
    for i in range(n):
        d = max(0, min(d + random.choice((-2, -1, 0, 0, 1, 1)), 5)) if l else 0
        l.append((random.choice(('', '_', 'abc', 'x_y', '__', 'a much longer title than that', 'ü_')), random.randrange(1 << 40), d, random.choice((0, 0, 0, 1, 2, 4, 8, 16, 24, 30, 31)), random.choice(('', '', 'a', 'F12', 'a key that is quite long')), random.randrange(16)))
    do('MenuSetItems', '(ia(sxiusu))', m, l)
    call(unique, '/Menu1', M, 'GetLayout', '(iias)', 0, -1, [])
    top = drv('MenuChildren', '(ii)', m, 0); out('top', top)
    l2 = [(random.choice((t[0], t[0] + 'x')), t[1], t[2], t[3] ^ random.choice((0, 0, 2, 4, 8)), random.choice((t[4], 'k')), random.choice((t[5], 2))) for t in l]
    do('MenuSetItems', '(ia(sxiusu))', m, l2)
# a second one without callbacks, one without a bus
m2 = do('MenuNew', '(bb)', True, False)
do('MenuSetPath', '(is)', m2, '/Other')
call(unique, '/Other', M, 'GetLayout', '(iias)', 0, -1, [])
call(unique, '/Other', M, 'AboutToShow', '(i)', 0)
do('MenuSetItems', '(ia(sxiusu))', m2, items)
call(unique, '/Other', M, 'AboutToShow', '(i)', 1); event('/Other', 1, 'clicked')
m3 = do('MenuNew', '(bb)', False, True)
do('MenuSetItems', '(ia(sxiusu))', m3, items); do('MenuSetPath', '(is)', m3, '/Nowhere'); do('MenuDelete', '(i)', m3)
do('MenuSetPath', '(is)', m, '')
# gone while a click waits
def hold(messages):
    sync(); os.kill(prog.pid, signal.SIGSTOP)
    for path, iface, member, sig, args in messages:
        msg = Gio.DBusMessage.new_method_call(unique, path, iface, member)
        if sig: msg.set_body(GLib.Variant(sig, args))
        msg.set_flags(Gio.DBusMessageFlags.NO_REPLY_EXPECTED)
        bus.send_message(msg, Gio.DBusSendMessageFlags.NONE)
    bus.flush_sync(None); time.sleep(.3)
    os.kill(prog.pid, signal.SIGCONT); time.sleep(.2); sync()
do('MenuSetItems', '(ia(sxiusu))', m2, items)
clickable = [('one', 0x3001, 0, 0, '', 0), ('two', 0x3002, 0, 0, '', 0)]
def click(i): return ('/Going%d' % i, M, 'Event', '(isvu)', (1, 'clicked', V('s', ''), 0))
def going():
    i = do('MenuNew', '(bb)', True, True); do('MenuSetItems', '(ia(sxiusu))', i, clickable); do('MenuSetPath', '(is)', i, '/Going%d' % i); return i
out('two clicks'); g = going(); hold([click(g), click(g)])
out('a click and then gone'); g = going(); hold([click(g), ('/driver', 'test.Driver', 'MenuDelete', '(i)', (g,))])
out('gone and then a click'); g2 = going(); hold([('/driver', 'test.Driver', 'MenuDelete', '(i)', (g2,)), click(g2)])
out('a click and then other items'); g3 = going(); hold([click(g3), ('/driver', 'test.Driver', 'MenuSetItems', '(ia(sxiusu))', (g3, []))])
out('other items and then a click'); g4 = going(); hold([('/driver', 'test.Driver', 'MenuSetItems', '(ia(sxiusu))', (g4, clickable[:1])), click(g4)])
call(unique, '/Going%d' % g, M, 'GetLayout', '(iias)', 0, -1, [])
do('MenuDelete', '(i)', m); call(unique, '/Menu1', M, 'GetLayout', '(iias)', 0, -1, [])
introspect(unique, '/')

# ---------------------------------------------------------------- now playing
out('=== now playing')
A, B = 'org.mpris.MediaPlayer2', 'org.mpris.MediaPlayer2.Player'
MP = '/org/mpris/MediaPlayer2'
def state(app='App', title=None, artist=None, album=None, art=None, duration=0., position=0., rate=1., status=1, commands=0x3ff, track=0):
    return [(x is not None, x or '') for x in (app, title, artist, album, art)], duration, position, rate, status, commands, track
def update(i, **k): return do('PlayingUpdate', '(ia(bs)dddiut)', i, *state(**k))
def all_playing():
    props(unique, MP, A); props(unique, MP, B)
    out('inner', drv('PlayingStatus', '(i)', n), drv('PlayingPosition', '(i)', n))
def owned():
    r = bus.call_sync('org.freedesktop.DBus', '/', 'org.freedesktop.DBus', 'ListNames', None, None, 0, 5000, None)
    out('names', sorted(clean(x) for x in r.unpack()[0] if 'mpris' in x))
drv('SetClock', '(x)', 1000000000000)
n = do('PlayingNew', '(bs)', True, 'org.example.My-App')
out('inner', drv('PlayingStatus', '(i)', n), drv('PlayingPosition', '(i)', n))
update(n, status=0); owned()
do('PlayingWithdraw', '(i)', n)
update(n, title='Song', artist='Artist', album='Album', art='/tmp/a b%20.png', duration=123.4567894, position=10.25, track=42); owned()
introspect(unique, MP); all_playing()
for i, l in ((A, ('Identity', 'DesktopEntry', 'CanQuit', 'CanRaise', 'HasTrackList', 'SupportedUriSchemes', 'SupportedMimeTypes', 'Fullscreen')), (B, ('PlaybackStatus', 'LoopStatus', 'Rate', 'MinimumRate', 'MaximumRate', 'Metadata', 'Volume', 'Position', 'CanGoNext', 'CanGoPrevious', 'CanPlay', 'CanPause', 'CanSeek', 'CanControl'))):
    for p in l: get(unique, MP, i, p)
put(unique, MP, B, 'Rate', V('d', 2.)); put(unique, MP, B, 'PlaybackStatus', V('s', 'Paused')); put(unique, MP, A, 'Identity', V('s', 'x'))
def commands(track=42):
    for mask in (0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 3, 5, 6, 7, 0xc0, 0x3ff, 0xffffffff):
        for st in (1, 2, 3, 4, -1):
            drv('PlayingUpdate', '(ia(bs)dddiut)', n, *state(title='Song', duration=100., position=10., status=st, commands=mask, track=track)); sync()
            out('mask', mask, 'status', st)
            for member in ('Play', 'Pause', 'PlayPause', 'Stop', 'Next', 'Previous'): call(unique, MP, B, member); sync()
            for member in ('Raise', 'Quit'): call(unique, MP, A, member); sync()
            for o in (0, 1500000, -5000000, -50000000, 9000000000000): call(unique, MP, B, 'Seek', '(x)', o); sync()
            for t, p in (('/org/cutefish/reef/item/%d' % track, 0), ('/org/cutefish/reef/item/%d' % track, 99999999), ('/org/cutefish/reef/item/%d' % track, 100000000), ('/org/cutefish/reef/item/%d' % track, 100000001), ('/org/cutefish/reef/item/%d' % track, -1),
                         ('/org/cutefish/reef/item/43', 5), ('/org/cutefish/reef/item', 5), ('/org/mpris/MediaPlayer2/TrackList/NoTrack', 5)): call(unique, MP, B, 'SetPosition', '(ox)', t, p); sync()
            for p in ('CanGoNext', 'CanGoPrevious', 'CanPlay', 'CanPause', 'CanSeek', 'CanControl', 'PlaybackStatus', 'Position'): get(unique, MP, B, p)
            for p in ('CanQuit', 'CanRaise'): get(unique, MP, A, p)
commands()
for member, sig, args in (('Play', '(i)', (1,)), ('Seek', None, ()), ('Seek', '(i)', (1,)), ('SetPosition', '(sx)', ('/a', 1)), ('SetPosition', '(o)', ('/a',)), ('SetPosition', None, ()), ('OpenUri', '(s)', ('file:///x',)), ('OpenUri', None, ()), ('OpenUri', '(i)', (3,)), ('Nothing', None, ())):
    call(unique, MP, B, member, sig, *args); sync()
call(unique, MP, A, 'Raise', '(s)', 'x'); sync()
# time passes
update(n, title='Song', duration=100., position=10., rate=1., track=1)
for t in (0, 1, 999, 500000000, 1500000001, 89999999999, 90000000000, 200000000000, -5000000000, -20000000000):
    drv('SetClock', '(x)', 1000000000000 + t); get(unique, MP, B, 'Position')
drv('SetClock', '(x)', 1000000000000)
for rate in (2., .5, 0., -1., 1e300, float('inf'), float('nan')):
    for st in (1, 2):
        for duration in (0., 30., -5.):
            drv('SetClock', '(x)', 1000000000000)
            update(n, title='Song', duration=duration, position=20., rate=rate, status=st, track=1)
            drv('SetClock', '(x)', 1005000000000)
            get(unique, MP, B, 'Rate'); get(unique, MP, B, 'Metadata'); get(unique, MP, B, 'Position'); out('inner', drv('PlayingPosition', '(i)', n))
for position in (-1., 0., 1e-7, 4.9999995e-7, 5e-7, 1.5e-6, 1e12, float('nan')):
    drv('SetClock', '(x)', 1000000000000)
    update(n, title='Song', duration=0., position=position, rate=1., status=2, track=1); get(unique, MP, B, 'Position')
    update(n, title='Song', duration=position, position=1., rate=1., status=2, track=1); get(unique, MP, B, 'Metadata'); get(unique, MP, B, 'Position')
# each field alone
base = dict(app='App', title='T', artist='A', album='L', art='/x', duration=50., position=5., rate=1., status=1, commands=0xff, track=9)
def changed(**k):
    drv('SetClock', '(x)', 1000000000000); update(n, **base); out('change', k); update(n, **dict(base, **k))
for k in (dict(app='Other'), dict(app=None), dict(app=''), dict(title='U'), dict(title=None), dict(title=''), dict(artist='B'), dict(artist=None), dict(album='M'), dict(album=None), dict(art='/y'), dict(art=''), dict(art=None), dict(art='http://x'), dict(duration=51.), dict(duration=0.), dict(position=5.9), dict(position=6.), dict(position=6.000001), dict(position=4.), dict(position=3.9), dict(position=50.),
          dict(rate=0.), dict(rate=2.), dict(status=2), dict(status=3), dict(status=7), dict(commands=0), dict(commands=0xfe), dict(commands=0x1ff), dict(commands=0x2ff), dict(commands=0x3ff), dict(commands=0x4ff), dict(track=10), dict(track=10, position=20.), dict(track=0), dict(track=(1 << 64) - 1), dict(title='U', position=30.), dict(status=2, rate=0., title='V', commands=3, app='W'),
          dict(title='a long title, longer than fifteen', artist='a long artist, longer than fifteen', album='a long album, longer than fifteen', art='/a/long/path/longer/than/fifteen.png'), dict(title='é世')):
    changed(**k); get(unique, MP, B, 'Metadata'); get(unique, MP, A, 'Identity')
# the clock moves between the updates, as it does
for moved in (0, 900000000, 1000000000, 1000001000, 3000000000):
    drv('SetClock', '(x)', 1000000000000); update(n, **base); drv('SetClock', '(x)', 1000000000000 + moved); out('moved', moved); update(n, **dict(base, position=5. + moved / 1e9)); update(n, **base)
update(n, status=0); owned(); introspect(unique, MP); get(unique, MP, B, 'Position')
update(n, **dict(base, app=None)); owned(); all_playing()
do('PlayingWithdraw', '(i)', n); owned(); do('PlayingWithdraw', '(i)', n)
update(n, **base); owned(); all_playing()
do('PlayingDelete', '(i)', n); owned(); get(unique, MP, B, 'Position')
# names
for entry, app in (('', 'My App 1.0'), ('', ''), ('', None), ('9lives', 'x'), ('_x', 'x'), ('-', 'x'), ('a' * 40, 'x'), ('', 'äöü café'), ('a.b-c d/e', 'x'), ('0', 'x'), ('1234567890123456', 'x'), ('', '7')):
    n = do('PlayingNew', '(bs)', True, entry); update(n, app=app); owned(); get(unique, MP, A, 'DesktopEntry'); get(unique, MP, A, 'Identity'); do('PlayingDelete', '(i)', n); owned()
n = do('PlayingNew', '(bs)', False, 'nobus'); update(n, **base); out('inner', drv('PlayingStatus', '(i)', n), drv('PlayingPosition', '(i)', n)); update(n, **dict(base, position=7.)); do('PlayingWithdraw', '(i)', n); update(n, status=0); do('PlayingDelete', '(i)', n)
# two at once
n = do('PlayingNew', '(bs)', True, 'same'); n2 = do('PlayingNew', '(bs)', True, 'same')
update(n, **base); owned(); update(n2, **dict(base, title='second')); owned(); all_playing(); do('PlayingDelete', '(i)', n2); owned(); do('PlayingDelete', '(i)', n); owned()
# the name belongs to somebody else
other = connect(); names[other.get_unique_name()] = ':OTHER'
other.call_sync('org.freedesktop.DBus', '/', 'org.freedesktop.DBus', 'RequestName', GLib.Variant('(su)', ('org.mpris.MediaPlayer2.taken.instance%d' % prog.pid, 4)), None, 0, 5000, None); time.sleep(.1); sync()
n = do('PlayingNew', '(bs)', True, 'taken'); update(n, **base); owned(); all_playing(); update(n, **dict(base, title='still')); do('PlayingDelete', '(i)', n); owned()
drv('SetClock', '(x)', -1)

# ---------------------------------------------------------------- status items
out('=== status items')
S, W = 'org.kde.StatusNotifierItem', 'org.kde.StatusNotifierWatcher'
XML = '<node><interface name="org.kde.StatusNotifierWatcher"><method name="RegisterStatusNotifierItem"><arg type="s" direction="in"/></method></interface></node>'
info = Gio.DBusNodeInfo.new_for_xml(XML).interfaces[0]
class Watcher:
    def __init__(self, letter):
        self.c = connect(); names[self.c.get_unique_name()] = ':WATCHER' + letter
        def method(c, sender, path, iface, member, params, inv): out('  watcher', letter, sender, path, iface, member, show(params)); inv.return_value(None)
        self.r = self.c.register_object('/StatusNotifierWatcher', info, method, None, None)
        self.own()
    def own(self): self.c.call_sync('org.freedesktop.DBus', '/', 'org.freedesktop.DBus', 'RequestName', GLib.Variant('(su)', (W, 0)), None, 0, 5000, None); time.sleep(.1); sync()
    def release(self): self.c.call_sync('org.freedesktop.DBus', '/', 'org.freedesktop.DBus', 'ReleaseName', GLib.Variant('(s)', (W,)), None, 0, 5000, None); time.sleep(.1); sync()
def settle(): time.sleep(.1); sync()
s = do('StatusNew', '(bxbs)', True, 0xc0de, True, 'org.example.item'); settle()
path = '/StatusNotifierItem/1'
introspect(unique, path); introspect(unique, '/StatusNotifierItem')
props(unique, path, S)
for p in ('Category', 'Id', 'Title', 'Status', 'WindowId', 'IconThemePath', 'IconName', 'IconPixmap', 'OverlayIconName', 'OverlayIconPixmap', 'AttentionIconName', 'AttentionIconPixmap', 'AttentionMovieName', 'ToolTip', 'ItemIsMenu', 'Menu', 'Nothing'): get(unique, path, S, p)
put(unique, path, S, 'Title', V('s', 'x')); put(unique, path, S, 'Status', V('s', 'Passive'))
for member in ('Activate', 'SecondaryActivate', 'ContextMenu'):
    for x, y in ((0, 0), (10, -20), (2147483647, -2147483648)): call(unique, path, S, member, '(ii)', x, y); sync()
    for sig, args in ((None, ()), ('(i)', (1,)), ('(uu)', (1, 2)), ('(iii)', (1, 2, 3)), ('(dd)', (1., 2.))): call(unique, path, S, member, sig, *args); sync()
for sig, args in (('(is)', (120, 'vertical')), ('(is)', (-1, '')), (None, ()), ('(i)', (1,)), ('(si)', ('a', 1))): call(unique, path, S, 'Scroll', sig, *args); sync()
w = Watcher('1'); settle(); w.release(); settle(); w.own(); settle()
w2 = Watcher('2'); settle(); w.release(); settle(); w2.release(); settle(); w2.own(); settle()
for t in ('Title', 'Title', '', '', 'a title that is longer than fifteen characters', 'a title that is longer than fifteen characters', 'a title that is longer than fifteen characterS', 'ä'):
    do('StatusSetTitle', '(is)', s, t); get(unique, path, S, 'Title')
for t in ('Tip', 'Tip', '', '', 'a tip that is longer than fifteen characters', 'a tip that is longer than fifteen characters', 'x'):
    do('StatusSetToolTip', '(is)', s, t); get(unique, path, S, 'ToolTip')
def icon(given, pixels, w, h):
    drv('StatusSetIcon', '(ibauii)', s, given, pixels, w, h); out('icon', given, len(pixels), w, h); sync(); get(unique, path, S, 'IconPixmap')
icon(True, [0xff000000, 0xffffffff, 0x80808080, 0x00000000], 2, 2)
icon(True, [0x01000000, 0x01010101, 0x02010000, 0x7f7f3f00, 0x00ffffff, 0x80ffffff, 0x10203040, 0xfe7f0180], 4, 2)
icon(False, [], 2, 2); icon(True, [1, 2], 0, 2); icon(True, [1, 2], 2, 0); icon(True, [1, 2], -1, 2); icon(True, [1, 2], 2, -1); icon(False, [], 0, 0)
icon(True, [a << 24 | c << 16 | c << 8 | c for a in range(0, 256, 5) for c in range(0, 256, 5)], 52, 52)
for k in range(8):
    width, height = random.randrange(1, 40), random.randrange(1, 40)
    icon(True, [random.randrange(1 << 32) if random.random() < .8 else random.randrange(1 << 24) for _ in range(width * height)], width, height)
mm = do('MenuNew', '(bb)', True, True); do('MenuSetItems', '(ia(sxiusu))', mm, items)
do('StatusSetMenu', '(ii)', s, 0); props(unique, path, S)
do('StatusSetMenu', '(ii)', s, mm); get(unique, path, S, 'Menu'); get(unique, path, S, 'ItemIsMenu')
introspect(unique, path); call(unique, path + '/Menu', M, 'GetLayout', '(iias)', 0, -1, [])
do('StatusSetMenu', '(ii)', s, mm); do('StatusSetMenu', '(ii)', s, 0); get(unique, path, S, 'Menu'); get(unique, path, S, 'ItemIsMenu')
mn = do('MenuNew', '(bb)', True, True); do('MenuSetPath', '(is)', mn, '/Elsewhere'); do('StatusSetMenu', '(ii)', s, mn); get(unique, path, S, 'Menu')
mz = do('MenuNew', '(bb)', False, True); do('StatusSetMenu', '(ii)', s, mz); get(unique, path, S, 'Menu'); get(unique, path, S, 'ItemIsMenu'); do('StatusSetMenu', '(ii)', s, 0)
# the second, third and so on
s2 = do('StatusNew', '(bxbs)', True, 0, False, ''); settle(); props(unique, '/StatusNotifierItem/2', S); call(unique, '/StatusNotifierItem/2', S, 'Activate', '(ii)', 1, 2); sync()
s3 = do('StatusNew', '(bxbs)', False, 1, True, 'no bus'); settle(); do('StatusSetTitle', '(is)', s3, 'x'); do('StatusSetToolTip', '(is)', s3, 'x'); s, keep = s3, s; drv('StatusSetIcon', '(ibauii)', s3, True, [1, 2, 3, 4], 2, 2); sync(); s = keep
do('StatusSetMenu', '(ii)', s3, mz); do('StatusDelete', '(i)', s3)
s4 = do('StatusNew', '(bxbs)', True, 0x44, True, 'a long identifier of a status item'); settle(); props(unique, '/StatusNotifierItem/4', S)
do('StatusSetTitle', '(is)', s4, 'four'); do('StatusSetMenu', '(ii)', s4, mm); get(unique, '/StatusNotifierItem/4', S, 'Menu')
introspect(unique, '/StatusNotifierItem')
for k in range(9):
    t = do('StatusNew', '(bxbs)', True, k, True, 'n%d' % k); settle(); call(unique, '/StatusNotifierItem/%d' % (5 + k), S, 'Activate', '(ii)', k, k); sync(); do('StatusDelete', '(i)', t)
out('activated and gone'); hold([('/StatusNotifierItem/4', S, 'Activate', '(ii)', (3, 4)), ('/driver', 'test.Driver', 'StatusDelete', '(i)', (s4,))])
call(unique, '/StatusNotifierItem/4', S, 'Activate', '(ii)', 1, 2)
out('activated and renamed'); hold([('/StatusNotifierItem/1', S, 'Activate', '(ii)', (3, 4)), ('/driver', 'test.Driver', 'StatusSetTitle', '(is)', (s, 'again'))])
do('StatusDelete', '(i)', s); w2.release(); settle(); w2.own(); settle()
introspect(unique, '/')

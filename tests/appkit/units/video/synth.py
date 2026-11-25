# Case files with headers that encoders do not write: synth.py FOLDER SEED COUNT [WEIRD], WEIRD 1 for wrong values and changes of the parameter sets
import os, random, struct, sys
from mp4cases import write

class W:
    def __init__(self): self.b = []
    def u(self, n, v):
        v &= (1 << n) - 1
        for i in range(n - 1, -1, -1): self.b.append(v >> i & 1)
        return self
    def f(self, v): self.b.append(1 if v else 0); return v
    def ue(self, v):
        v += 1
        n = v.bit_length()
        self.u(2 * n - 1, v)
    def se(self, v): self.ue(2 * v - 1 if v > 0 else -2 * v)
    def trailing(self):
        self.b.append(1)
        while len(self.b) % 8: self.b.append(0)
    def align(self, bit):
        while len(self.b) % 8: self.b.append(bit)
    def bytes(self):
        b = self.b + [0] * (-len(self.b) % 8)
        return bytes(sum(b[i + j] << (7 - j) for j in range(8)) for i in range(0, len(b), 8))

def escape(d):
    out = bytearray(); zeros = 0
    for x in d:
        if zeros >= 2 and x <= 3:
            out.append(3); zeros = 0
        out.append(x)
        zeros = zeros + 1 if x == 0 else 0
    return bytes(out)

def units(list_, n):
    return b''.join(len(u).to_bytes(n, 'big') + u for u in list_)

R = random.Random()
def chance(p): return R.random() < p
WEIRD = 0.0
def bad(p): return WEIRD > 0 and R.random() < p * WEIRD
def tail():
    # Zeros are more frequent, for the escapes.
    return bytes(R.choice([0, 0, 0, 1, 2, 3, R.randrange(256)]) for _ in range(R.choice([3, 20, 60, 400, 1500])))

# ---- H.264

def lists264(w, count):
    for i in range(count):
        if not w.f(chance(0.6)): continue
        size = 16 if i < 6 else 64
        last = next_ = 8
        mode = R.random()
        for j in range(size):
            if next_ != 0:
                if mode < 0.15 and j == 0: delta = -8
                elif mode < 0.4 and j == R.randrange(1, size): delta = -last
                elif chance(0.02): delta = R.choice([-128, 127, -100, 90])
                else: delta = R.randint(-4, 8)
                delta = max(-128, min(127, delta))
                if bad(0.0005): delta = R.choice([-129, 128, 5000])
                w.se(delta)
                next_ = (last + delta + 256) % 256
                if j == 0 and next_ == 0: break
            last = last if next_ == 0 else next_

def sps264(s):
    w = W()
    w.u(8, 0x67); w.u(8, s['profile']); w.u(8, R.randrange(256)); w.u(8, R.choice([10, 30, 31, 40, 51]))
    w.ue(s['id'] if not bad(0.02) else 32)
    if s['profile'] in (100, 110, 122, 244, 44, 83, 86, 118, 128, 138, 139, 134, 135):
        w.ue(s['chroma'] if not bad(0.02) else 4)
        if s['chroma'] == 3: w.f(s['planes'])
        w.ue(s['depth'] - 8); w.ue(s['depth'] - 8)
        w.f(chance(0.5))
        if w.f(s['lists']): lists264(w, 8 if s['chroma'] != 3 else 12)
    w.ue(s['logFrame'] - 4)
    w.ue(s['pocType'])
    if s['pocType'] == 0: w.ue(s['logPoc'] - 4 if not bad(0.04) else 13)
    elif s['pocType'] == 1:
        s['alwaysZero'] = w.f(s['alwaysZero'])
        w.se(R.randint(-5, 5)); w.se(R.randint(-5, 5))
        n = R.choice([0, 1, 2, 5, 20])
        if bad(0.04): n = 256
        w.ue(n)
        for i in range(n): w.se(R.randint(-10, 10))
    w.ue(s['refs']); w.f(s['gaps']); w.ue(s['wmb'] - 1); w.ue(s['hmb'] - 1)
    w.f(s['frameOnly'])
    if not s['frameOnly']: w.f(s['mbaff'])
    w.f(chance(0.5))
    if w.f(chance(0.5)):
        for i in range(4): w.ue(R.choice([0, 1, 2, 5]))
    w.f(0)
    w.trailing()
    return escape(w.bytes())

def pps264(p, s):
    w = W()
    w.u(8, 0x68); w.ue(p['id'] if not bad(0.03) else 256); w.ue(p['sps'])
    w.f(p['cabac']); w.f(p['bottom'])
    w.ue(p.get('groups', 0))
    w.ue(p['counts'][0] - 1); w.ue(p['counts'][1] - 1)
    w.f(p['wp']); w.u(2, p['wb'])
    w.se(R.randint(-20, 20)); w.se(R.randint(-20, 20)); w.se(R.randint(-12, 12))
    w.f(p['deblock']); w.f(chance(0.5)); w.f(p['redundant'])
    if p['extra']:
        t = w.f(chance(0.7))
        if w.f(chance(0.5)): lists264(w, 6 + ((2 if s['chroma'] != 3 else 6) if t else 0))
        w.se(R.randint(-12, 12))
    w.trailing()
    return escape(w.bytes())

def slice264(s, p, a, first):
    w = W()
    idr = a['idr']
    w.u(8, (a['ref'] << 5) | (5 if idr else 1))
    w.ue(0 if first else R.randrange(1, 40))
    t = a['types'][0 if first else R.randrange(len(a['types']))]
    w.ue(t + (5 if chance(0.3) else 0))
    w.ue(p['id'] if not bad(0.02) else 77)
    if s['planes'] and s['chroma'] == 3: w.u(2, 0)
    w.u(s['logFrame'], a['frame'])
    if not s['frameOnly']: w.f(0)
    if idr: w.ue(a['idrId'])
    if s['pocType'] == 0:
        w.u(s['logPoc'], a['poc'])
        if p['bottom']: w.se(a['bottom'])
    elif s['pocType'] == 1 and not s['alwaysZero']:
        w.se(a['delta'][0])
        if p['bottom']: w.se(a['delta'][1])
    if p['redundant']: w.ue(0)
    if t == 1: w.f(chance(0.5))
    counts = list(p['counts'])
    if t in (0, 1, 3):
        if w.f(chance(0.5)):
            counts[0] = R.choice([1, 1, 2, 3, 5, 16, 32]); w.ue(counts[0] - 1)
            if t == 1: counts[1] = R.choice([1, 1, 2, 3, 5, 32]); w.ue(counts[1] - 1)
    for l in range(0 if t in (2, 4) else 2 if t == 1 else 1):
        if w.f(chance(0.3)):
            for i in range(R.choice([0, 1, 1, 2, 3, 6])):
                op = R.choice([0, 0, 0, 1, 1, 2])
                if chance(0.01): op = R.choice([4, 5, 6])
                w.ue(op)
                w.ue(R.choice([0, 0, 0, 1, 1, 2, 3, 5, 20]) if op != 2 else R.choice([0, 0, 1, 2]))
            w.ue(3)
    if (p['wp'] and t in (0, 3)) or (p['wb'] == 1 and t == 1):
        w.ue(R.randrange(8)); w.ue(R.randrange(8))
        for l in range(2 if t == 1 else 1):
            for i in range(counts[l]):
                if w.f(chance(0.4)): w.se(R.randint(-128, 127)); w.se(R.randint(-128, 127))
                if w.f(chance(0.4)):
                    for j in range(4): w.se(R.randint(-128, 127))
    if a['ref']:
        if idr: w.f(chance(0.3)); w.f(a['longIdr'])
        elif w.f(a['mmco'] is not None):
            for m in a['mmco']:
                w.ue(m[0])
                for x in m[1:]: w.ue(x)
            w.ue(0)
    if p['cabac'] and t not in (2, 4): w.ue(R.randrange(3))
    w.se(R.randint(-20, 20))
    if t in (3, 4):
        if t == 3: w.f(0)
        w.se(R.randint(-20, 20))
    if p['deblock']:
        d = R.choice([0, 0, 1, 2]); w.ue(d)
        if d != 1: w.se(R.randint(-6, 6)); w.se(R.randint(-6, 6))
    if p['cabac']: w.align(1)
    else: w.u(R.randrange(8), R.randrange(256))
    return escape(w.bytes() + tail())

def case264(path):
    n = R.choice([1, 2, 4, 4])
    s = dict(profile=R.choice([66, 77, 100, 100, 100]), id=R.choice([0, 0, 0, 3, 31]), chroma=1, planes=False, depth=8, lists=chance(0.5),
             logFrame=R.choice([4, 4, 5, 8, 16]), pocType=R.choice([0, 0, 1, 2]), logPoc=R.choice([4, 5, 8, 16]), alwaysZero=chance(0.3),
             refs=R.choice([0, 1, 2, 3, 4, 8, 16]), gaps=chance(0.4), wmb=R.randint(1, 30), hmb=R.randint(1, 30), frameOnly=True, mbaff=False)
    if chance(0.06): s.update(R.choice([dict(chroma=0, profile=100), dict(chroma=3, planes=True, profile=100), dict(chroma=2, profile=100),
                                        dict(depth=10, profile=100), dict(frameOnly=False), dict(refs=17), dict(profile=88), dict(id=32),
                                        dict(wmb=2000), dict(logFrame=20), dict(profile=110), dict(profile=244, chroma=3)]))
    ps = []
    for i in range(R.choice([1, 1, 2, 4])):
        ps.append(dict(id=R.choice([i, i, i * 3 + 1]), sps=s['id'], cabac=chance(0.5), bottom=chance(0.5),
                       counts=[R.choice([1, 2, 3, 8, 32]), R.choice([1, 2, 5])], wp=chance(0.5), wb=R.choice([0, 1, 2]),
                       deblock=chance(0.6), redundant=chance(0.2), extra=chance(0.5) and s['profile'] == 100))
    if chance(0.04): ps[0]['groups'] = 1
    if chance(0.04): ps[0]['sps'] = 9
    if chance(0.04): ps[0]['counts'][0] = 33
    sb = sps264(s); pb = [pps264(p, s) for p in ps]
    count = len(pb)
    config = bytes([1, s['profile'], 0, 30, 0xfc | (n - 1), 0xe0 | 1]) + struct.pack('>H', len(sb)) + sb + bytes([count]) \
        + b''.join(struct.pack('>H', len(x)) + x for x in pb)
    samples = []
    frame = 0; poc = 0; prevRef = 0; lastWasRef = False
    for k in range(R.randint(4, 40)):
        idr = (k == 0 and not chance(0.1)) or chance(0.05)
        a = dict(idr=idr, ref=R.choice([0, 1, 2, 3, 3]) if not idr else R.choice([1, 2, 3]), idrId=R.randrange(4), longIdr=chance(0.3))
        if idr: frame = 0; poc = 0
        elif k == 0: frame = R.randrange(1 << s['logFrame'])
        elif lastWasRef: frame = (frame + 1) % (1 << s['logFrame'])
        if not idr and chance(0.06): frame = (frame + R.choice([1, 2, 3, 7])) % (1 << s['logFrame'])
        a['frame'] = frame
        if not idr: poc += R.choice([2, 2, 4, 6, -2, 1])
        if chance(0.03): poc += R.choice([100, -100, 9])
        a['poc'] = poc % (1 << s['logPoc']) if not idr else R.choice([0, 0, 2])
        a['bottom'] = R.randint(-3, 3); a['delta'] = [R.randint(-3, 3), R.randint(-3, 3)]
        a['types'] = [2] if idr and chance(0.8) else R.choice([[0], [0], [1], [1, 0], [2], [0, 2], [0, 1, 2], [3, 4], [1]])
        a['mmco'] = None
        if chance(0.35):
            m = []
            for i in range(R.choice([0, 1, 1, 2, 3])):
                op = R.choice([1, 1, 1, 2, 3, 3, 4, 4, 5 if chance(0.3) else 1, 6, 6])
                if op == 1: m.append((1, R.choice([0, 0, 1, 2, 3])))
                elif op == 2: m.append((2, R.choice([0, 0, 1, 2])))
                elif op == 3: m.append((3, R.choice([0, 0, 1, 2]), R.choice([0, 0, 1, 2])))
                elif op == 4: m.append((4, R.choice([0, 1, 2, 3])))
                elif op == 5: m.append((5,))
                else: m.append((6, R.choice([0, 0, 1, 2])))
            a['mmco'] = m
        lastWasRef = a['ref'] != 0
        us = []
        if chance(0.1): us.append(bytes([9, 0xf0]))
        if chance(0.1): us.append(bytes([6, 1, 2, 3, 0x80]))
        if chance(0.03): us.append(sb)
        if chance(0.03): us.append(R.choice(pb))
        if chance(0.02): us.append(sps264(s)); sb = us[-1]
        if chance(0.02):
            i = R.randrange(len(ps)); us.append(pps264(ps[i], s)); pb[i] = us[-1]
        if bad(0.08):
            # Another kind of picture from here on.
            s = dict(s); s.update(R.choice([dict(chroma=3, planes=True, profile=100), dict(wmb=7), dict(frameOnly=False, mbaff=chance(0.5)), dict(depth=9, profile=100),
                                            dict(chroma=0, profile=100), dict(refs=1), dict(pocType=R.choice([0, 1, 2])), dict(logFrame=6)]))
            sb = sps264(s); us.append(sb)
            if chance(0.5):
                pb = [pps264(p, s) for p in ps]; us += pb
        for i in range(R.choice([1, 1, 1, 2, 3])):
            us.append(slice264(s, R.choice(ps), a, i == 0))
        if n == 1: us = [u[:255] for u in us]
        samples.append(units(us, n))
        if a['mmco'] and lastWasRef and any(m[0] == 5 for m in a['mmco']): frame = 0; poc = 0
    write(path, 0x61766331, 0, 0, config, samples)

# ---- HEVC

def lists265(w):
    for size in range(4):
        for m in range(0, 6, 3 if size == 3 else 1):
            if not w.f(chance(0.5)):
                w.ue(R.randint(0, m // (3 if size == 3 else 1)) if not chance(0.01) else 7)
            else:
                next_ = 8
                if size > 1:
                    d = R.randint(-7, 30)
                    if bad(0.005): d = R.choice([-8, -20, 248])
                    w.se(d); next_ = d + 8
                for i in range(min(64, 1 << (4 + (size << 1)))):
                    d = R.randint(-4, 8) if not chance(0.02) else R.choice([-128, 127])
                    w.se(d); next_ = (next_ + d + 256) % 256

def rps(w, sets, index, count):
    # The deltas and whether they are used, as a list of pairs for both directions.
    if index and w.f(chance(0.5)):
        back = 1
        if index == count:
            back = R.randint(1, min(index, 3)); w.ue(back - 1)
        n, p = sets[index - back]
        sign = chance(0.5); ab = R.choice([1, 1, 2, 3, 8])
        w.f(sign); w.ue(ab - 1 if not bad(0.01) else 0x8000)
        delta = -ab if sign else ab
        L = len(n); T = L + len(p)
        used = []; use = []
        for j in range(T + 1):
            x = w.f(chance(0.6)); y = True
            if not x: y = w.f(chance(0.6))
            used.append(bool(x)); use.append(y)
        neg = [(p[j][0] + delta, used[L + j], use[L + j]) for j in range(len(p) - 1, -1, -1)] + [(delta, used[T], use[T])] \
            + [(n[j][0] + delta, used[j], use[j]) for j in range(L)]
        pos = [(n[j][0] + delta, used[j], use[j]) for j in range(L - 1, -1, -1)] + [(delta, used[T], use[T])] \
            + [(p[j][0] + delta, used[L + j], use[L + j]) for j in range(len(p))]
        return [x[:2] for x in neg if x[0] < 0 and x[2]][:16], [x[:2] for x in pos if x[0] > 0 and x[2]][:16]
    n = R.choice([0, 1, 1, 2, 3, 4, 8]); p = R.choice([0, 0, 1, 2, 3])
    w.ue(n if not bad(0.005) else 17); w.ue(p if not bad(0.005) else 15)
    neg = []; pos = []
    d = 0
    for i in range(n):
        x = R.choice([1, 1, 1, 2, 3]); d -= x; w.ue(x - 1); neg.append((d, bool(w.f(chance(0.8)))))
    d = 0
    for i in range(p):
        x = R.choice([1, 1, 1, 2, 3]); d += x; w.ue(x - 1); pos.append((d, bool(w.f(chance(0.8)))))
    return neg, pos

def sps265(s):
    w = W()
    w.u(16, 0x4201); w.u(4, 0); w.u(3, s['sub']); w.f(1)
    w.u(2, 0); w.f(0); w.u(5, s['profile']); w.u(32, 0x60000000); w.u(48, 0x900000000000); w.u(8, 93)
    pp = []; lp = []
    for i in range(s['sub']): pp.append(w.f(chance(0.5))); lp.append(w.f(chance(0.5)))
    if s['sub']:
        for i in range(s['sub'], 8): w.u(2, 0)
    for i in range(s['sub']):
        if pp[i]: w.u(88, R.getrandbits(88))
        if lp[i]: w.u(8, 90)
    w.ue(s['id']); w.ue(s['chroma'])
    if s['chroma'] == 3: w.f(s['planes'])
    w.ue(s['w']); w.ue(s['h'])
    if w.f(chance(0.5)):
        for i in range(4): w.ue(R.choice([0, 1, 2, 7]))
    w.ue(s['depth'] - 8); w.ue(s['cdepth'] - 8); w.ue(s['logPoc'] - 4)
    all_ = w.f(chance(0.5))
    for i in range(0 if all_ else s['sub'], s['sub'] + 1):
        w.ue(s['dpb'] - 1); w.ue(R.choice([0, 1, 2])); w.ue(R.choice([0, 1, 5]))
    w.ue(s['minCb'] - 3); w.ue(s['ctb'] - s['minCb']); w.ue(0); w.ue(R.randint(0, 3)); w.ue(R.randint(0, 3)); w.ue(R.randint(0, 3))
    if w.f(s['lists']):
        if w.f(chance(0.6)): lists265(w)
    w.f(chance(0.5)); w.f(s['sao'])
    if w.f(chance(0.3)):
        w.u(4, R.randrange(8)); w.u(4, R.randrange(8)); w.ue(R.randrange(2)); w.ue(R.randrange(3)); w.f(chance(0.5))
    s['sets'] = []
    n = R.choice([0, 1, 2, 5, 12, 64])
    w.ue(n if not bad(0.03) else 65)
    for i in range(n):
        s['sets'].append(rps(w, s['sets'], i, n))
    s['lt'] = w.f(chance(0.6)); s['ltc'] = 0; s['ltu'] = []
    if s['lt']:
        s['ltc'] = R.choice([0, 1, 2, 3, 32]); w.ue(s['ltc'] if not bad(0.03) else 33)
        for i in range(s['ltc']): w.u(s['logPoc'], R.randrange(16)); s['ltu'].append(bool(w.f(chance(0.7))))
    s['tmvp'] = w.f(chance(0.6)); w.f(chance(0.5))
    w.f(0); w.f(0)
    w.trailing()
    return escape(w.bytes())

def pps265(p, s):
    w = W()
    w.u(16, 0x4401); w.ue(p['id']); w.ue(p['sps'])
    w.f(p['dep']); w.f(p['out']); w.u(3, p['extra']); w.f(chance(0.5)); w.f(p['cabac'])
    w.ue(p['counts'][0] - 1); w.ue(p['counts'][1] - 1); w.se(R.randint(-26, 25)); w.f(chance(0.5)); w.f(chance(0.5))
    if w.f(chance(0.5)): w.ue(R.randint(0, 3))
    w.se(R.randint(-12, 12)); w.se(R.randint(-12, 12)); w.f(p['chromaQp']); w.f(p['wp']); w.f(p['wb']); w.f(chance(0.3))
    w.f(p['tiles']); w.f(p['sync'])
    if p['tiles']:
        c = p['cols']; r = p['rows']
        w.ue(c - 1); w.ue(r - 1)
        if not w.f(p['uniform']):
            for i in range(c - 1): w.ue(R.choice([0, 1, 2]))
            for i in range(r - 1): w.ue(R.choice([0, 1, 2]))
        w.f(chance(0.5))
    w.f(p['across'])
    p['override'] = p['disabled'] = False
    if w.f(p['deblock']):
        p['override'] = w.f(chance(0.5)); p['disabled'] = w.f(chance(0.4))
        if not p['disabled']: w.se(R.randint(-6, 6)); w.se(R.randint(-6, 6))
    if w.f(p['lists']): lists265(w)
    w.f(p['mod']); w.ue(R.randint(0, 4)); w.f(p['ext'])
    w.f(0)
    w.trailing()
    return escape(w.bytes())

def log2(n):
    b = 0
    while (1 << b) < n: b += 1
    return b

def slice265(s, p, a, first, dependent):
    if bad(0.02): p = dict(p); p['id'] = 33
    w = W()
    t = a['nal']
    w.f(0); w.u(6, t); w.u(6, 0); w.u(3, a['tid'] + 1)
    w.f(first)
    if 16 <= t <= 23: w.f(chance(0.5))
    w.ue(p['id'])
    ctb = 1 << s['ctb']
    blocks = ((s['w'] + ctb - 1) // ctb) * ((s['h'] + ctb - 1) // ctb)
    if not first:
        if p['dep']: w.f(dependent)
        w.u(log2(blocks), R.randrange(max(blocks, 1)))
    if first or not (p['dep'] and dependent):
        w.u(p['extra'], R.randrange(256))
        ty = R.choice(a['types'])
        w.ue(ty if not bad(0.02) else 3)
        if p['out']: w.f(chance(0.8))
        if s['planes'] and s['chroma'] == 3: w.u(2, 0)
        used = 0; tm = False
        if t not in (19, 20):
            w.u(s['logPoc'], a['poc'] % (1 << s['logPoc']))
            n = len(s['sets'])
            if not w.f(a['fromSps'] and (n > 0 or chance(0.05))):
                x, y = rps(w, s['sets'], n, n)
            else:
                i = a['set'] % max(n, 1)
                if n > 1:
                    w.u(log2(n), i if not chance(0.02) else (1 << log2(n)) - 1)
                x, y = s['sets'][i] if n else ([], [])
            used = sum(e[1] for e in x + y)
            if s['lt']:
                sc = 0
                if s['ltc'] > 0:
                    sc = R.choice([0, 0, 1, min(2, s['ltc'])]); w.ue(sc)
                lc = R.choice([0, 0, 1, 2]); w.ue(lc)
                for i in range(sc + lc):
                    if i < sc:
                        k = R.randrange(s['ltc'])
                        if s['ltc'] > 1: w.u(log2(s['ltc']), k if not bad(0.1) else (1 << log2(s['ltc'])) - 1)
                        used += s['ltu'][k]
                    else:
                        w.u(s['logPoc'], R.choice(a['past']) % (1 << s['logPoc'])); used += bool(w.f(chance(0.7)))
                    if w.f(chance(0.4)): w.ue(R.choice([0, 0, 0, 1]))
            if s['tmvp']: tm = w.f(chance(0.7))
        sao = False
        if s['sao']:
            sao = w.f(chance(0.5))
            if s['chroma'] and not (s['chroma'] == 3 and s['planes']): sao = w.f(chance(0.5)) or sao
        if ty != 2:
            counts = list(p['counts'])
            if w.f(chance(0.5)):
                counts[0] = R.choice([1, 1, 2, 3, 5, 15]); w.ue(counts[0] - 1)
                if ty == 0: counts[1] = R.choice([1, 1, 2, 3, 15]); w.ue(counts[1] - 1)
            if p['mod'] and used > 1:
                for l in range(2 if ty == 0 else 1):
                    if w.f(chance(0.5)):
                        for i in range(counts[l]): w.u(log2(used), R.randrange(used) if not chance(0.01) else used)
            if ty == 0: w.f(chance(0.5))
            if p['cabac']: w.f(chance(0.5))
            if tm:
                l0 = True
                if ty == 0: l0 = w.f(chance(0.5))
                if counts[0 if l0 else 1] > 1: w.ue(R.randrange(counts[0 if l0 else 1]))
            if (p['wp'] and ty == 1) or (p['wb'] and ty == 0):
                w.ue(R.randrange(8)); w.se(R.randint(-2, 2))
                for l in range(2 if ty == 0 else 1):
                    lf = [w.f(chance(0.5)) for i in range(counts[l])]
                    cf = [w.f(chance(0.5)) for i in range(counts[l])]
                    for i in range(counts[l]):
                        if lf[i]: w.se(R.randint(-128, 127)); w.se(R.randint(-128, 127))
                        if cf[i]:
                            for j in range(2): w.se(R.randint(-128, 127)); w.se(R.randint(-512, 511))
            w.ue(R.randrange(5))
        w.se(R.randint(-20, 20))
        if p['chromaQp']: w.se(R.randint(-5, 5)); w.se(R.randint(-5, 5))
        off = p['disabled']
        if p['override'] and w.f(chance(0.5)):
            off = w.f(chance(0.5))
            if not off: w.se(R.randint(-6, 6)); w.se(R.randint(-6, 6))
        if p['across'] and (sao or not off): w.f(chance(0.5))
    if p['tiles'] or p['sync']:
        n = R.choice([0, 0, 1, 3, 10]); w.ue(n if not bad(0.02) else 4401)
        if n:
            l = R.choice([1, 8, 16, 32]); w.ue(l - 1 if not bad(0.02) else 32)
            for i in range(n): w.u(l, R.getrandbits(l))
    if p['ext']:
        n = R.choice([0, 1, 5]); w.ue(n)
        for i in range(n): w.u(8, R.randrange(256))
    w.trailing()
    if bad(0.03): return escape(w.bytes())[:R.randint(3, 12)]
    return escape(w.bytes() + tail())

def case265(path):
    n = R.choice([1, 2, 4, 4])
    depth = R.choice([8, 8, 10])
    ctb = R.choice([4, 5, 6])
    s = dict(sub=R.choice([0, 0, 1, 2, 6]), profile=2 if depth == 10 else R.choice([1, 2, 3]), id=R.choice([0, 0, 2, 15]), chroma=1, planes=False,
             w=R.choice([16, 64, 416, 1000]), h=R.choice([16, 64, 240, 700]), depth=depth, cdepth=depth, logPoc=R.choice([4, 5, 8, 16]),
             dpb=R.choice([1, 2, 3, 5, 6, 16]), minCb=3, ctb=ctb, lists=chance(0.5), sao=chance(0.6))
    if chance(0.06): s.update(R.choice([dict(chroma=0), dict(chroma=3, planes=True), dict(chroma=2), dict(cdepth=9), dict(profile=4), dict(depth=12, cdepth=12),
                                        dict(id=16), dict(w=0), dict(dpb=17), dict(ctb=7), dict(sub=7)]))
    sb = sps265(s)
    ps = []
    for i in range(R.choice([1, 1, 2, 4])):
        tiles = chance(0.3)
        ps.append(dict(id=R.choice([i, i, i * 3 + 1]), sps=s['id'], dep=chance(0.5), out=chance(0.3), extra=R.choice([0, 0, 1, 3]), cabac=chance(0.5),
                       counts=[R.choice([1, 2, 3, 15]), R.choice([1, 2, 5])], chromaQp=chance(0.5), wp=chance(0.5), wb=chance(0.5), tiles=tiles,
                       sync=chance(0.3), cols=R.choice([1, 2, 3, 20]), rows=R.choice([1, 2, 4, 22]), uniform=chance(0.5), across=chance(0.5),
                       deblock=chance(0.6), lists=chance(0.3), mod=chance(0.4), ext=chance(0.3)))
    if chance(0.04): ps[0]['sps'] = 9
    if chance(0.04): ps[0]['counts'][0] = 16
    if chance(0.03): ps[0]['tiles'] = True; ps[0]['cols'] = 21
    if chance(0.03): ps[0]['id'] = 64
    pb = [pps265(p, s) for p in ps]
    vps = bytes([0x40, 1, 0xc, 1, 0xff])
    arrays = [(32, [vps]), (33, [sb]), (34, pb)]
    if chance(0.2): arrays.append((39, [bytes([0x4e, 1, 5, 4, 3])]))
    if bad(0.03): arrays.append((33, [bytes([0x42, 1])]))
    if bad(0.03): arrays = [a for a in arrays if a[0] != R.choice([33, 34])]
    config = bytes([1]) + bytes(R.randrange(256) for _ in range(20)) + bytes([0xfc | (n - 1), len(arrays)])
    for t, us in arrays:
        config += bytes([0x80 | t]) + struct.pack('>H', len(us)) + b''.join(struct.pack('>H', len(u)) + u for u in us)
    samples = []
    poc = 0; past = [0]
    for k in range(R.randint(4, 40)):
        if k == 0 and not chance(0.1): nal = R.choice([16, 17, 18, 19, 19, 20, 21, 21])
        elif chance(0.08): nal = R.choice([16, 17, 18, 19, 20, 21, 21, 22, 23])
        else: nal = R.choice([0, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 14, 10, 15, 24, 31])
        a = dict(nal=nal, tid=R.choice([0, 0, 0, 1, 2]) if nal < 16 else 0)
        if nal in (19, 20): poc = 0; past = [0]
        else: poc += R.choice([1, 1, 1, 2, 3, -1, -2])
        if chance(0.02): poc += R.choice([100, -100])
        poc = max(poc, 0)
        a['poc'] = poc
        a['past'] = past[-8:]
        a['types'] = [2] if 16 <= nal <= 23 and chance(0.8) else R.choice([[0], [1], [0, 1], [0, 1, 2], [2]])
        a['fromSps'] = chance(0.6); a['set'] = R.randrange(64)
        us = []
        if chance(0.1): us.append(bytes([0x46, 1, 0x50]))
        if chance(0.05): us.append(bytes([0x4e, 1, 5, 4, 3]))
        if chance(0.03): us.append(sb)
        if chance(0.03): us.append(R.choice(pb))
        if chance(0.02): sb = sps265(s); us.append(sb)
        if chance(0.02):
            i = R.randrange(len(ps)); pb[i] = pps265(ps[i], s); us.append(pb[i])
        if bad(0.08):
            s = dict(s); s.update(R.choice([dict(chroma=3, planes=True), dict(w=128), dict(depth=10, cdepth=10), dict(chroma=0), dict(chroma=2), dict(sub=7), dict(logPoc=6), dict(dpb=2)]))
            sb = sps265(s); us.append(sb)
            if chance(0.5):
                pb = [pps265(q, s) for q in ps]; us += pb
        p = R.choice(ps)
        for i in range(R.choice([1, 1, 1, 2, 4])):
            us.append(slice265(s, p if not chance(0.05) else R.choice(ps), a, i == 0 and not chance(0.01), chance(0.5)))
        if chance(0.05): us.append(bytes([0x48, 1, 0]))  # end of the sequence
        if n == 1: us = [u[:255] for u in us]
        if chance(0.01): us.insert(0, b'\x02')
        samples.append(units(us, n))
        past.append(poc)
    write(path, 0x68766331, 0, 0, config, samples)

# ---- VP9

def color9(w, h):
    if h['profile'] >= 2: w.f(h['depth'] == 12)
    space = 7 if chance(0.02) else R.choice([0, 1, 2, 5])
    w.u(3, space)
    if space != 7:
        w.f(0)
        if h['profile'] in (1, 3): w.f(chance(0.5)); w.f(chance(0.5)); w.f(0)
    elif h['profile'] in (1, 3): w.f(0)

def frame9(st):
    w = W()
    profile = st['profile']
    w.u(2, 2 if not chance(0.01) else 1); w.u(1, profile & 1); w.u(1, profile >> 1)
    if profile == 3: w.f(0)
    if w.f(chance(0.03) and st['started']):
        w.u(3, R.randrange(8))
        return w.bytes()
    key = not st['started'] and not chance(0.05) or chance(0.07)
    w.f(not key); show = w.f(chance(0.8)); err = w.f(chance(0.2))
    intra = False
    if key:
        w.u(24, 0x498342 if not chance(0.01) else 0x498343)
        color9(w, st)
        if chance(0.3) or not st['started']: st['w'] = R.choice(st['sizes']); st['h'] = R.choice(st['sizes'])
        w.u(16, st['w'] - 1); w.u(16, st['h'] - 1)
        if w.f(chance(0.2)): w.u(32, R.getrandbits(32))
        st['started'] = True
        refresh = 255
    else:
        if not show: intra = w.f(chance(0.3))
        if not err: w.u(2, R.randrange(4))
        if intra:
            w.u(24, 0x498342)
            if profile > 0: color9(w, st)
            refresh = R.choice([0, 1, 255, R.randrange(256)])
            w.u(8, refresh)
            if chance(0.3): st['w'] = R.choice(st['sizes']); st['h'] = R.choice(st['sizes'])
            w.u(16, st['w'] - 1); w.u(16, st['h'] - 1)
            if w.f(chance(0.2)): w.u(32, R.getrandbits(32))
        else:
            refresh = R.choice([0, 1, 2, 4, R.randrange(256)])
            w.u(8, refresh)
            refs = []
            for i in range(3): refs.append(R.randrange(8)); w.u(3, refs[-1]); w.f(chance(0.4))
            k = R.choice([0, 0, 0, 1, 2, 3])
            for i in range(min(k + 1, 3)): w.f(i == k)
            if k == 3:
                if chance(0.5): st['w'] = R.choice(st['sizes']); st['h'] = R.choice(st['sizes'])
                w.u(16, st['w'] - 1); w.u(16, st['h'] - 1)
            else: st['w'], st['h'] = st['refs'][refs[k]]
            if w.f(chance(0.2)): w.u(32, R.getrandbits(32))
            w.f(chance(0.5))
            if not w.f(chance(0.4)): w.u(2, R.randrange(4))
    if not err: w.f(chance(0.5)); w.f(chance(0.5))
    w.u(2, R.randrange(4))
    w.u(6, R.randrange(64)); w.u(3, R.randrange(8))
    if w.f(chance(0.7)):
        if w.f(chance(0.5)):
            for i in range(6):
                if w.f(chance(0.5)): w.u(6, R.randrange(64)); w.f(chance(0.5))
    w.u(8, R.choice([0, 0, 1, 40, 128, 255, R.randrange(256)]))
    for i in range(3):
        if w.f(chance(0.2)): w.u(4, R.randrange(16)); w.f(chance(0.5))
    if w.f(chance(0.5)):
        if w.f(chance(0.5)):
            for i in range(7):
                if w.f(chance(0.5)): w.u(8, R.randrange(256))
            if w.f(chance(0.5)):
                for i in range(3):
                    if w.f(chance(0.5)): w.u(8, R.randrange(256))
        if w.f(chance(0.5)):
            w.f(chance(0.4))
            for i in range(8):
                for j, n in enumerate((8, 6, 2, 0)):
                    if w.f(chance(0.4)):
                        w.u(n, R.getrandbits(8))
                        if j < 2: w.f(chance(0.5))
    blocks = (((st['w'] + 7) >> 3) + 7) >> 3
    lo = 0
    while (64 << lo) < blocks: lo += 1
    hi = 1
    while (blocks >> hi) >= 4: hi += 1
    hi -= 1
    while lo < hi:
        lo += 1
        if not w.f(chance(0.5)): break
    if w.f(chance(0.5)): w.f(chance(0.5))
    data = tail()
    w.u(16, len(data) if chance(0.5) else R.choice([1, len(data), len(data), len(data) + 1, 0]) if chance(0.2) else R.randint(1, len(data)))
    w.align(0)
    for i in range(8):
        if refresh >> i & 1: st['refs'][i] = (st['w'], st['h'])
    return w.bytes() + data

def case9(path):
    profile = R.choice([0, 0, 0, 2, 2])
    depth = 10 if profile == 2 else 8
    st = dict(profile=profile, depth=depth, started=False, refs=[(0, 0)] * 8, w=64, h=64,
              sizes=R.choice([[64], [320, 64], [1, 8, 33, 64, 65, 320], [200, 1000, 5000, 65536], [4096, 4097, 8192]]))
    if chance(0.03): st['profile'] = R.choice([1, 3])
    if chance(0.03): st['depth'] = 12
    config = bytes([1, 0, 0, 0, profile, 30, depth << 4 | R.choice([0, 2, 2, 3]), 1, 1, 1, 0, 0])
    if chance(0.05):
        c = bytearray(config); c[R.randrange(12)] = R.randrange(256); config = bytes(c)
    if chance(0.02): config = config[:11]
    samples = [frame9(st) for k in range(R.randint(4, 40))]
    write(path, 0x76703039, R.choice([64, 320, 1, 0 if chance(0.1) else 2]), R.choice([64, 240, 1, -1 if chance(0.1) else 2]), config, samples)

if __name__ == '__main__':
    folder, seed, count = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    WEIRD = float(sys.argv[4]) if len(sys.argv) > 4 else 0.0
    os.makedirs(folder, exist_ok=True)
    for i in range(count):
        for name, make in (('h264', case264), ('hevc', case265), ('vp9', case9)):
            R.seed(seed * 1000003 + i)
            make('%s/%s_%04d.case' % (folder, name, i))

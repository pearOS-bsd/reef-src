# Turns the MP4 files of a folder into case files for the driver.
# case: "VCAS", u32 codec, i32 width, i32 height, u32 configuration length, bytes, u32 count, {u32 length, bytes}*
import glob, os, struct, sys

def boxes(d, start, end):
    while start + 8 <= end:
        size, kind = struct.unpack('>I4s', d[start:start + 8])
        head = 8
        if size == 1:
            size = struct.unpack('>Q', d[start + 8:start + 16])[0]
            head = 16
        elif size == 0:
            size = end - start
        yield kind, start + head, start + size
        start += size

def find(d, start, end, *path):
    for kind, a, b in boxes(d, start, end):
        if kind == path[0]:
            return (a, b) if len(path) == 1 else find(d, a, b, *path[1:])
    return None

def write(path, codec, width, height, config, samples):
    with open(path, 'wb') as f:
        f.write(b'VCAS' + struct.pack('<IiiI', codec, width, height, len(config)) + config + struct.pack('<I', len(samples)))
        for s in samples:
            f.write(struct.pack('<I', len(s)) + s)

def parse(path):
    d = open(path, 'rb').read()
    stbl = find(d, 0, len(d), b'moov', b'trak', b'mdia', b'minf', b'stbl')
    a, b = find(d, *stbl, b'stsd')
    entry = a + 8
    size, codec = struct.unpack('>I4s', d[entry:entry + 8])
    width, height = struct.unpack('>HH', d[entry + 32:entry + 36])
    config = None
    for kind, x, y in boxes(d, entry + 8 + 78, entry + size):
        if kind in (b'avcC', b'hvcC', b'vpcC'):
            config = d[x:y]
    a, b = find(d, *stbl, b'stsz')
    fixed, count = struct.unpack('>II', d[a + 4:a + 12])
    sizes = [fixed] * count if fixed else list(struct.unpack('>%dI' % count, d[a + 12:a + 12 + 4 * count]))
    r = find(d, *stbl, b'stco')
    if r:
        n = struct.unpack('>I', d[r[0] + 4:r[0] + 8])[0]
        chunks = struct.unpack('>%dI' % n, d[r[0] + 8:r[0] + 8 + 4 * n])
    else:
        r = find(d, *stbl, b'co64')
        n = struct.unpack('>I', d[r[0] + 4:r[0] + 8])[0]
        chunks = struct.unpack('>%dQ' % n, d[r[0] + 8:r[0] + 8 + 8 * n])
    a, b = find(d, *stbl, b'stsc')
    n = struct.unpack('>I', d[a + 4:a + 8])[0]
    runs = [struct.unpack('>III', d[a + 8 + 12 * i:a + 20 + 12 * i]) for i in range(n)]
    samples = []
    for i, chunk in enumerate(chunks):
        per = [r[1] for r in runs if r[0] <= i + 1][-1]
        for j in range(per):
            if len(samples) < count:
                s = sizes[len(samples)]
                samples.append(d[chunk:chunk + s])
                chunk += s
    return codec, width, height, config, samples

def split(sample):
    # The frames of a VP9 superframe.
    last = sample[-1]
    if last & 0xe0 == 0xc0:
        frames = (last & 7) + 1
        magnitude = (last >> 3 & 3) + 1
        index = 2 + magnitude * frames
        if len(sample) >= index and sample[-index] == last:
            out, position = [], 0
            for i in range(frames):
                x = len(sample) - index + 1 + i * magnitude
                size = int.from_bytes(sample[x:x + magnitude], 'little')
                out.append(sample[position:position + size])
                position += size
            return out
    return [sample]

if __name__ == '__main__':
    folder = sys.argv[1]
    codes = {b'avc1': 0x61766331, b'hvc1': 0x68766331, b'vp09': 0x76703039}
    n = 0
    for path in sorted(p for p in glob.glob(folder + "/*.mp4") if os.path.getsize(p) > 100):
        codec, width, height, config, samples = parse(path)
        base = path[:-4]
        write(base + '.case', codes[codec], width, height, config, samples)
        n += 1
        if codec == b'vp09':
            write(base + '_split.case', codes[codec], width, height, config, [f for s in samples for f in split(s)])
            n += 1
    print(n, 'cases')

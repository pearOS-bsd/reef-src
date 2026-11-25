# pictures.py DIR : on the host (needs Pillow): the pictures of the decode test and the icon themes of the icon test
import os, random, sys
from PIL import Image
random.seed(7)
D = sys.argv[1]; P = D + '/pictures'; os.makedirs(P, exist_ok=True)
def noise(mode, w, h):
    n = {'RGBA': 4, 'RGB': 3, 'L': 1, 'LA': 2, 'CMYK': 4}[mode]
    im = Image.new(mode, (w, h))
    px = im.load()
    for y in range(h):
        for x in range(w):
            v = tuple((x * 7 + y * 13 + c * 50 + random.randrange(40)) % 256 for c in range(n))
            px[x, y] = v if n > 1 else v[0]
    return im
for mode in ('RGBA', 'RGB', 'L', 'LA'):
    noise(mode, 31, 17).save(P + '/%s.png' % mode.lower())
noise('RGBA', 1, 1).save(P + '/one.png')
noise('RGBA', 300, 2).save(P + '/wide.png')
noise('RGB', 40, 30).quantize(16).save(P + '/palette.png')
noise('L', 33, 9).convert('1').save(P + '/bit.png')
noise('L', 20, 20).convert('I;16').save(P + '/gray16.png')
noise('RGB', 50, 40).save(P + '/baseline.jpg', quality=80)
noise('RGB', 50, 40).save(P + '/progressive.jpg', quality=70, progressive=True)
noise('L', 50, 40).save(P + '/gray.jpg')
noise('CMYK', 24, 24).save(P + '/cmyk.jpg')
ex = Image.Exif(); ex[0x0112] = 6
noise('RGB', 50, 20).save(P + '/turned.jpg', exif=ex.tobytes())
noise('RGBA', 40, 30).save(P + '/lossless.webp', lossless=True)
noise('RGB', 40, 30).save(P + '/lossy.webp', quality=60)
noise('RGB', 40, 30).quantize(32).save(P + '/one.gif')
f = [noise('RGB', 20, 20).quantize(16) for _ in range(3)]
f[0].save(P + '/moving.gif', save_all=True, append_images=f[1:], duration=100)
noise('RGB', 23, 11).save(P + '/rgb.bmp')
noise('RGBA', 48, 48).save(P + '/several.ico', sizes=[(16, 16), (32, 32), (48, 48)])
noise('RGB', 20, 20).save(P + '/rgb.tiff')
noise('RGB', 20, 20).save(P + '/rgb.ppm')
SVG = '<svg xmlns="http://www.w3.org/2000/svg" %s>%s</svg>'
shapes = '<rect x="2" y="3" width="10" height="7" fill="#c03020"/><circle cx="15" cy="15" r="6" fill="#2060c0" fill-opacity=".6"/>'
open(P + '/plain.svg', 'w').write(SVG % ('width="40" height="30" viewBox="0 0 24 24"', shapes))
open(P + '/nosize.svg', 'w').write(SVG % ('viewBox="0 0 24 24"', shapes))
open(P + '/nobox.svg', 'w').write(SVG % ('width="20" height="20"', shapes))
open(P + '/bad.svg', 'w').write('<svg width="20" height="20"><rect')
open(P + '/text.txt', 'w').write('This is no picture.\n')
open(P + '/empty', 'w').write('')
# Icon themes: in XDG_DATA_HOME/icons
I = D + '/data/icons'
def icon(path, size=None, svg=None, color=(200, 80, 40, 255)):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    if svg is not None: open(path, 'w').write(svg)
    else:
        im = noise('RGBA', size, size); px = im.load()
        for i in range(size): px[i, i] = color
        im.save(path)
for theme, parents, names in (('Cutefish', 'hicolor', ('folder', 'only-scalable', 'only-16', 'text-x-generic', 'x-office')), ('second', 'Cutefish', ('folder', 'image-x-generic')), ('hicolor', '', ('app.example.Thing', 'a', 'with space', 'folder'))):
    T = I + '/' + theme
    os.makedirs(T, exist_ok=True)
    dirs = ['16x16/apps', '22x22/apps', '32x32/apps', '48x48/apps', '48x48@2/apps', 'scalable/apps']
    with open(T + '/index.theme', 'w') as f:
        f.write('[Icon Theme]\nName=%s\nComment=test\nDirectories=%s\n' % (theme, ','.join(dirs)))
        if parents: f.write('Inherits=%s\n' % parents)
        for d in dirs:
            size = 48 if d.startswith('scal') else int(d.split('x')[0])
            f.write('\n[%s]\nSize=%d\nContext=Applications\n' % (d, size))
            if '@2' in d: f.write('Scale=2\nType=Fixed\n')
            elif d.startswith('scal'): f.write('Type=Scalable\nMinSize=8\nMaxSize=512\n')
            else: f.write('Type=%s\n' % ('Threshold' if size < 40 else 'Fixed'))
    for n in names:
        if n == 'only-scalable': icon(T + '/scalable/apps/%s.svg' % n, svg=SVG % ('width="48" height="48" viewBox="0 0 24 24"', shapes)); continue
        if n == 'only-16': icon(T + '/16x16/apps/%s.png' % n, 16); continue
        for s in (16, 22, 32, 48):
            if (len(n) + s) % 3: icon(T + '/%dx%d/apps/%s.png' % (s, s, n), s)
        icon(T + '/48x48@2/apps/%s.png' % n, 96)
        if len(n) % 2: icon(T + '/scalable/apps/%s.svg' % n, svg=SVG % ('viewBox="0 0 24 24"', shapes))
    icon(T + '/32x32/apps/broken.png', svg='no picture')
    icon(T + '/32x32/apps/empty.png', svg='')

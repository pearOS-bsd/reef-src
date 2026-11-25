# Fonts that the collection lacks: odd name tables, several strikes, depths of bitmaps, colour layers. usage: make.py FONTSET OUT (needs fontTools)
import sys, os, io, copy, random
from fontTools.ttLib import TTFont, newTable
from fontTools.ttLib.tables._n_a_m_e import NameRecord
from fontTools import subset
src, out = sys.argv[1], sys.argv[2]
def small(path, text=None, number=-1, **kw):
    f = TTFont(path, fontNumber=number)
    o = subset.Options(); o.name_IDs = ['*']; o.notdef_outline = True; o.layout_features = ['*']; o.glyph_names = True
    for k, v in kw.items(): setattr(o, k, v)
    s = subset.Subsetter(o); s.populate(text=text or ''.join(map(chr, range(32, 127)))); s.subset(f)
    return f
lato = os.path.join(src, 'lato/Lato-Regular.ttf')
base = io.BytesIO(); small(lato).save(base)
def font(): base.seek(0); return TTFont(io.BytesIO(base.getvalue()))
def rec(i, p, e, l, raw):
    r = NameRecord(); r.nameID, r.platformID, r.platEncID, r.langID = i, p, e, l
    r.string = raw if isinstance(raw, bytes) else raw.encode('utf_16_be', 'surrogatepass')
    return r
def named(name, records, ps2=True, drop=()):
    f = font(); t = newTable('name'); t.names = records; f['name'] = t
    for d in drop: del f[d]
    # fontTools encodes what it can read; the bytes are to stay as they are
    f.save(os.path.join(out, name))
W = lambda i, s, l=0x409, e=1: rec(i, 3, e, l, s)
M = lambda i, s, l=0, e=0: rec(i, 1, e, l, s)
U = lambda i, s, e=3: rec(i, 0, e, 0, s)
high = bytes(range(128, 256))
ws = [W(1, 'Made \U0001F600 One'), W(2, 'Regular'), W(4, 'Made \U0001F600\U00010000\U0010FFFF One Full'), W(6, 'MadeOne-Regular'), W(16, 'Typo ☃ One'), W(17, 'Typo \ud83d Style')]
named('n-surrogates.ttf', ws)
named('n-lone.ttf', [W(1, 'Lone \ud83dx'), W(2, '\ude00 low'), W(4, 'end \ud83d'), W(6, 'Lone-\ud83d😀'), W(16, b'\x00A\x00B\x00'), W(17, b'\x00')])
named('n-mac.ttf', [M(1, b'Mac ' + high), M(2, b'Bold\x8e'), M(4, b'Mac Full ' + high[::-1]), M(6, b'MacOnly-Bold')])
named('n-macjapanese.ttf', [M(1, b'\x83\x7d\x83\x62\x83\x4e', 11, 1), M(1, b'German Mac', 2), M(2, b'Fett', 2), M(6, b'GermanMac-Fett', 2)])
named('n-japanese.ttf', [W(1, '日本語ファミリー', 0x411), W(2, '標準', 0x411), W(4, '日本語ファミリー 標準', 0x411), W(6, 'OnlyJapanese-Regular', 0x411)])
named('n-british.ttf', [W(1, 'British', 0x809), W(2, 'Oblique', 0x809), W(1, 'Deutsch', 0x407), W(2, 'Schräg', 0x407), W(6, 'British-Oblique', 0x809)])
named('n-order.ttf', [M(1, b'Mac First'), W(1, 'Japanese Second', 0x411), U(1, 'Unicode Third'), W(1, 'English Fourth'), M(2, b'MacStyle'), U(2, 'UniStyle'), U(6, 'Order-Unicode'), M(6, b'Order-Mac')])
named('n-unicode.ttf', [U(1, 'Unicode Only \U0001F600'), U(2, 'Italic', 4), U(4, 'Unicode Only Full'), U(6, 'UnicodeOnly-Italic')])
named('n-symbol.ttf', [W(1, 'Symbol Encoded', 0x409, 0), W(2, 'Regular', 0x409, 0), W(1, 'Ucs4 Encoded', 0x409, 10), W(6, 'SymbolEncoded', 0x409, 0)])
named('n-empty.ttf', [W(1, ''), W(2, ''), W(4, ''), W(6, '')])
named('n-odd.ttf', [W(1, b'\x00O\x00d\x00d\x00'), W(2, b'\x00R\x00e'), W(6, b'\x00O\x00-\x00R\x00')])
named('n-none.ttf', [])
named('n-iso.ttf', [rec(1, 2, 1, 0, 'Iso Ten'), rec(1, 2, 0, 0, b'Iso Ascii'), rec(2, 2, 1, 0, 'Regular'), rec(6, 2, 1, 0, 'IsoTen-Regular'), rec(1, 4, 0, 0, b'Custom')])
named('n-macfrench.ttf', [M(1, 'Français'.encode('mac_roman'), 1), M(1, b'English Mac', 0), M(1, b'Italiano', 3), M(2, b'Regular', 0), M(6, b'MacFrench', 0)])
named('n-typo.ttf', [W(1, 'Typo Light'), W(2, 'Regular'), W(16, 'Typo'), W(17, 'Light Condensed'), W(4, 'Typo Light Condensed'), W(6, 'Typo-LightCondensed'), W(21, 'WWS'), W(22, 'WwsLight'), W(0, 'copyright ©'), W(3, 'unique'), W(5, 'Version 1'), W(7, 'tm'), W(8, 'maker'), W(13, 'license'), W(18, 'compat'), W(25, 'prefix'), W(256, 'user')])
named('n-nodash.ttf', [W(1, 'No Dash'), W(2, 'Regular'), W(6, 'NoDash')])
named('n-noos2.ttf', [W(1, 'No Tables'), W(2, 'Regular')], drop=['OS/2', 'post'])
def changed(name, change):
    f = font(); n = name.rsplit('.', 1)[0]
    t = newTable('name'); t.names = [W(1, 'Made ' + n), W(2, 'Regular'), W(4, 'Made ' + n), W(6, 'Made-' + n)]; f['name'] = t
    change(f); f.save(os.path.join(out, name))
def os2(f, **kw):
    for k, v in kw.items(): setattr(f['OS/2'], k, v)
def hhea(f, **kw):
    for k, v in kw.items(): setattr(f['hhea'], k, v)
changed('m-typo.ttf', lambda f: os2(f, fsSelection=f['OS/2'].fsSelection | 128, sTypoAscender=777, sTypoDescender=-333, sTypoLineGap=55))
changed('m-nohhea.ttf', lambda f: (hhea(f, ascent=0, descent=0, lineGap=9), os2(f, sTypoAscender=600, sTypoDescender=-100, sTypoLineGap=3)))
changed('m-nothing.ttf', lambda f: (hhea(f, ascent=0, descent=0), os2(f, sTypoAscender=0, sTypoDescender=0)))
changed('m-typozero.ttf', lambda f: os2(f, fsSelection=f['OS/2'].fsSelection | 128, sTypoAscender=0, sTypoDescender=0))
changed('m-descent.ttf', lambda f: hhea(f, ascent=0, descent=-200, lineGap=-40))
changed('m-caps.ttf', lambda f: os2(f, sCapHeight=0, sxHeight=0))
changed('m-capsnegative.ttf', lambda f: os2(f, sCapHeight=-5, sxHeight=-6))
def noH(f):
    os2(f, sCapHeight=0, sxHeight=0)
    for t in f['cmap'].tables:
        for c in (ord('H'), ord('x')): t.cmap.pop(c, None)
changed('m-noh.ttf', noH)
changed('m-v1.ttf', lambda f: os2(f, version=1))
changed('m-v0.ttf', lambda f: os2(f, version=0))
changed('m-weight.ttf', lambda f: os2(f, usWeightClass=600, usWidthClass=9))
changed('m-weight0.ttf', lambda f: os2(f, usWeightClass=0, usWidthClass=0))
def post(f, **kw):
    for k, v in kw.items(): setattr(f['post'], k, v)
changed('m-post.ttf', lambda f: post(f, italicAngle=-12.5, underlinePosition=-77, underlineThickness=0, isFixedPitch=1))
changed('m-postthick.ttf', lambda f: post(f, italicAngle=7.25, underlinePosition=44, underlineThickness=-3))
def upem(f):
    f['head'].unitsPerEm = 1
changed('m-upem1.ttf', upem)
def upem16(f): f['head'].unitsPerEm = 16384
changed('m-upem16384.ttf', upem16)
def nodigit(f):
    for t in f['cmap'].tables: t.cmap.pop(ord('8'), None)
changed('m-noeight.ttf', nodigit)
def zeroeight(f):
    g = f.getBestCmap()[ord('8')]; f['hmtx'][g] = (0, 0)
changed('m-zeroeight.ttf', zeroeight)
def nocmap(f): del f['cmap']
changed('m-nocmap.ttf', nocmap)
def symbolcmap(f):
    from fontTools.ttLib.tables._c_m_a_p import CmapSubtable
    best = f.getBestCmap(); t = CmapSubtable.newSubtable(4); t.platformID, t.platEncID, t.language = 3, 0, 0
    t.cmap = {0xf000 + c: g for c, g in best.items() if c < 256}; f['cmap'].tables = [t]
changed('m-symbolcmap.ttf', symbolcmap)
def maccmap(f):
    from fontTools.ttLib.tables._c_m_a_p import CmapSubtable
    best = f.getBestCmap(); t = CmapSubtable.newSubtable(0); t.platformID, t.platEncID, t.language = 1, 0, 0
    t.cmap = {c: best.get(c, '.notdef') for c in range(256)}; f['cmap'].tables = [t]
changed('m-maccmap.ttf', maccmap)
def post3(f): f['post'].formatType = 3.0
changed('m-post3.ttf', post3)
# Strikes of colour.
emoji = small(os.path.join(src, 'color/NotoColorEmoji.ttf'), text='\U0001F600\U0001F44D\U0001F30D⚽', layout_features=[])
def strikes(name, sizes):
    buf = io.BytesIO(); emoji.save(buf); f = TTFont(io.BytesIO(buf.getvalue()))
    f['CBLC'].strikes; f['CBDT'].strikeData
    s0, d0 = f['CBLC'].strikes[0], f['CBDT'].strikeData[0]
    f['CBLC'].strikes, f['CBDT'].strikeData = [], []
    for n in sizes:
        s = copy.deepcopy(s0); s.bitmapSizeTable.ppemX = s.bitmapSizeTable.ppemY = n
        f['CBLC'].strikes.append(s); f['CBDT'].strikeData.append(copy.deepcopy(d0))
    t = newTable('name'); t.names = [W(1, 'Strikes ' + name[:-4]), W(2, 'Regular'), W(4, 'Strikes ' + name[:-4]), W(6, 'Strikes' + name[:-4])]; f['name'] = t
    f.save(os.path.join(out, name))
strikes('s-up.ttf', [20, 40, 109])
strikes('s-down.ttf', [109, 40, 20])
strikes('s-mixed.ttf', [40, 109, 20, 64, 160])
strikes('s-same.ttf', [50, 50])
# Bitmaps alone, of several depths.
from fontTools.ttLib.tables.E_B_D_T_ import ebdt_bitmap_format_1
from fontTools.ttLib.tables import E_B_L_C_, E_B_D_T_
from fontTools.ttLib.tables.BitmapGlyphMetrics import SmallGlyphMetrics
def bitmaps(name, depths, sizes, glyf=False):
    f = font(); n = name[:-4]
    t = newTable('name'); t.names = [W(1, 'Bitmaps ' + n), W(2, 'Regular'), W(4, 'Bitmaps ' + n), W(6, 'Bitmaps' + n)]; f['name'] = t
    order = f.getGlyphOrder(); names = [f.getBestCmap()[ord(c)] for c in 'ABCDab']
    ids = sorted(f.getGlyphID(g) for g in names); names = [order[i] for i in ids]
    lc, dt = newTable('EBLC'), newTable('EBDT'); lc.version = dt.version = 2.0; lc.strikes, dt.strikeData = [], []
    r = random.Random(5)
    for depth, ppem in zip(depths, sizes):
        s = E_B_L_C_.Strike(); b = s.bitmapSizeTable
        b.colorRef = 0; b.startGlyphIndex, b.endGlyphIndex, b.ppemX, b.ppemY, b.bitDepth, b.flags = ids[0], ids[-1], ppem, ppem, depth, 1
        for m in ('hori', 'vert'):
            l = E_B_L_C_.SbitLineMetrics(); setattr(b, m, l)
            for k in ('ascender', 'descender', 'widthMax', 'caretSlopeNumerator', 'caretSlopeDenominator', 'caretOffset', 'minOriginSB', 'minAdvanceSB', 'maxBeforeBL', 'minAfterBL', 'pad1', 'pad2'): setattr(l, k, 0)
            l.ascender, l.descender, l.widthMax = ppem * 3 // 4, -ppem // 4, ppem
        data = {}; tables = []
        for i, g in zip(ids, names):
            h, w = ppem * 2 // 3, 5 + (i % 3) * 4
            e = ebdt_bitmap_format_1.__new__(ebdt_bitmap_format_1); m = SmallGlyphMetrics(); m.height, m.width, m.BearingX, m.BearingY, m.Advance = h, w, 1 - i % 3, h - i % 4, w + 2
            e.metrics = m; e.imageData = bytes(r.randrange(256) for _ in range((w * depth + 7) // 8 * h)); e.ttFont = f
            data[g] = e
            sub = E_B_L_C_.eblc_index_sub_table_1.__new__(E_B_L_C_.eblc_index_sub_table_1); sub.ttFont = f; sub.indexFormat, sub.imageFormat = 1, 1; sub.firstGlyphIndex = sub.lastGlyphIndex = i; sub.names = [g]; sub.locations = [(0, 0)]
            tables.append(sub)
        s.indexSubTables = tables; lc.strikes.append(s); dt.strikeData.append(data)
    f['EBLC'], f['EBDT'] = lc, dt
    if not glyf:
        for t in ('glyf', 'loca', 'fpgm', 'prep', 'cvt ', 'GPOS', 'GSUB', 'GDEF', 'kern'):
            if t in f: del f[t]
    # no outlines: the compiler needs none of what depends on them
    f.recalcBBoxes = False; f.recalcTimestamp = False
    f.save(os.path.join(out, name))
for name, d, z in (('b-one.ttf', [1], [12]), ('b-two.ttf', [2], [16]), ('b-four.ttf', [4], [20]), ('b-eight.ttf', [8], [24]), ('b-all.ttf', [1, 2, 4, 8], [10, 20, 30, 40]), ('b-sizes.ttf', [1, 1, 1], [24, 9, 15])):
    try: bitmaps(name, d, z)
    except Exception as e: print(name, repr(e))
try: bitmaps('b-outlines.ttf', [8, 1], [16, 24], True)
except Exception as e: print('b-outlines', repr(e))
# Colour layers.
from fontTools.colorLib.builder import buildCOLR, buildCPAL
def layers(name, colr, version):
    f = font(); n = name[:-4]
    t = newTable('name'); t.names = [W(1, 'Layers ' + n), W(2, 'Regular'), W(4, 'Layers ' + n), W(6, 'Layers' + n)]; f['name'] = t
    f['COLR'] = buildCOLR(colr, version=version); f['CPAL'] = buildCPAL([[(1, 0, 0, 1), (0, 0.6, 0, 0.5), (0, 0, 1, 1)]]); f.save(os.path.join(out, name))
layers('c-v0.ttf', {'A': [('O', 0), ('A', 1)], 'B': [('B', 0xffff), ('I', 2)], 'C': [('D', 2)]}, 0)
try: layers('c-v1.ttf', {'A': {'Format': 10, 'Paint': {'Format': 2, 'PaletteIndex': 0, 'Alpha': 1.0}, 'Glyph': 'O'}, 'B': {'Format': 1, 'Layers': [{'Format': 10, 'Paint': {'Format': 2, 'PaletteIndex': 1, 'Alpha': 0.5}, 'Glyph': 'I'}]}}, 1)
except Exception as e: print('c-v1', repr(e))
# No outlines of CFF after the subset, and others.
cff = small(os.path.join(src, 'urw/P052-Roman.otf')); cff.save(os.path.join(out, 'o-cff.otf'))

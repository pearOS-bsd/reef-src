# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Turn the model of gen-skeleton.py (symbols, metadata, demangle trees) into Swift declarations."""
import collections, os, re, sys, json
from swifttree import N, gp_name
import areas

HEADER = '''// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// %s
'''
IMPORTS = ['Foundation', 'AppKit', 'CoreGraphics', 'CoreText', 'Combine', 'Observation', 'CoreTransferable',
           'UniformTypeIdentifiers', 'Symbols', 'DeveloperToolsSupport', 'Metal', 'os']

KW = set('''associatedtype class deinit enum extension fileprivate func import init inout internal let open operator private
precedencegroup protocol public rethrows static struct subscript typealias var break case catch continue default defer do else
fallthrough for guard if in repeat return throw switch where while as false is nil self Self super throws true try Any
Protocol Type some any'''.split())
MEMBER_OK = set('''Protocol Type some any Any'''.split())

# generic parameter names of types of other modules (extensions of them must use these names)
EXTERN_GP = {
    'Swift.Optional': ['Wrapped'], 'Swift.Array': ['Element'], 'Swift.Set': ['Element'], 'Swift.Dictionary': ['Key', 'Value'],
    'Swift.Range': ['Bound'], 'Swift.ClosedRange': ['Bound'], 'Swift.Result': ['Success', 'Failure'],
    'Swift.KeyPath': ['Root', 'Value'], 'Swift.WritableKeyPath': ['Root', 'Value'], 'Swift.ReferenceWritableKeyPath': ['Root', 'Value'],
    'Swift.PartialKeyPath': ['Root'], 'Swift.Slice': ['Base'], 'Swift.ArraySlice': ['Element'], 'Swift.ContiguousArray': ['Element'],
    'Swift.UnsafePointer': ['Pointee'], 'Swift.UnsafeMutablePointer': ['Pointee'], 'Swift.UnsafeBufferPointer': ['Element'],
    'Swift.UnsafeMutableBufferPointer': ['Element'], 'Swift.Unmanaged': ['Instance'],
    'Foundation.Measurement': ['UnitType'], 'Foundation.AttributeContainer': [], 'Foundation.SortDescriptor': ['Compared'],
    'Foundation.KeyPathComparator': ['Compared'], 'Swift.LazyMapSequence': ['Base', 'Element'], 'Swift.Never': [],
    'Swift.CollectionOfOne': ['Element'], 'Swift.EmptyCollection': ['Element'], 'Swift.Repeated': ['Element'],
    'Foundation.FormatStyle': [], 'Foundation.Date.FormatStyle': [],
}

STUB = '_skeleton()'


def esc(name):
    if name in KW: return '`%s`' % name
    return name


def esc_member(name):
    """a member name after a dot or in a declaration"""
    if name in KW and name not in MEMBER_OK: return '`%s`' % name
    return name


class Env:
    """how generic parameters print"""
    def __init__(self, names=None, proto_self=False, opaque=None, class_self=None):
        self.names = dict(names or {})
        self.proto_self = proto_self
        self.opaque = opaque
        self.class_self = class_self

    def child(self, **kw):
        e = Env(self.names, self.proto_self, self.opaque, self.class_self)
        for k, v in kw.items(): setattr(e, k, v)
        return e

    def gp(self, d, i):
        if self.proto_self and d == 0 and i == 0: return self.class_self or 'Self'
        return self.names.get((d, i)) or gp_name(d, i)


def idx(n):
    return int(n.t)


class TypeInfo:
    def __init__(self, **kw):
        self.__dict__.update(kw)
        self.members = []; self.nested = []; self.conformances = []; self.ext_blocks = collections.OrderedDict()


class Emitter:
    def __init__(self, g):
        self.g = g; self.m = g.m; self.meta = g.meta
        self.fixes = g.fixes
        self.renames = self.fixes.get('renames', {})
        self.exports = set()
        for name, ntype, nsect, value in self.m.symbols:
            if nsect and (ntype & 1) and not (ntype & 0xe0): self.exports.add(name)
        self.types = {}          # key -> TypeInfo
        self.type_by_addr = {}   # descriptor addr -> TypeInfo
        self.protocols = {}      # key -> TypeInfo (kind protocol)
        self.private_names = {}  # (disc, name, parentkey) -> emitted name
        self.used_names = collections.defaultdict(set)
        self.problems = collections.Counter()
        self.files = collections.defaultdict(list)

    # ------------------------------------------------------------------------------------------ keys
    def name_of(self, n):
        """declared name of an entity/type node (Identifier, PrivateDeclName, LocalDeclName ...)"""
        for c in n.c:
            if c.k == 'Identifier': return c.t
            if c.k == 'PrivateDeclName': return c.c[-1].t
            if c.k == 'LocalDeclName': return c.c[-1].t
            if c.k == 'RelatedEntityDeclName': return c.c[-1].t if c.c else c.t
            if c.k in ('InfixOperator', 'PrefixOperator', 'PostfixOperator'): return c.t
        return None

    def disc_of(self, n):
        for c in n.c:
            if c.k == 'PrivateDeclName': return c.c[0].t
            if c.k == 'LocalDeclName': return 'L' + c.c[0].t
        return None

    def key(self, n):
        """identity of a context node"""
        if n is None: return '?'
        k = n.k
        if k == 'Type': return self.key(n.c[0])
        if k == 'Module': return n.t
        if k in ('Structure', 'Class', 'Enum', 'Protocol', 'TypeAlias', 'OtherNominalType'):
            par = self.key(n.c[0])
            d = self.disc_of(n)
            nm = self.name_of(n)
            if n.c and n.c[0].k == 'Module' and n.c[0].t in ('__C', '__C_Synthesized'):
                rel = [c for c in n.c if c.k == 'RelatedEntityDeclName']
                if rel: nm = nm + '$' + rel[0].t
            return '%s.%s%s' % (par, nm, ('#' + d) if d else '')
        if k.startswith('BoundGeneric'):
            return self.key(n.c[0])
        if k == 'Extension':
            return self.key(n.c[1])
        if k == 'TypeSymbolicReference':
            t = self.ref_tree(int(n.t))
            return self.key(t) if t else '?ref'
        if k == 'ProtocolSymbolicReference':
            t = self.ref_tree(int(n.t), proto=True)
            return self.key(t) if t else '?ref'
        if k in ('Function', 'Getter', 'Variable', 'Allocator', 'ExplicitClosure', 'ImplicitClosure', 'Subscript', 'Static'):
            return 'local:' + n.key()
        if k == 'AnonymousContext':
            return self.key(n.c[1]) if len(n.c) > 1 else '?anon'
        return '?' + k

    def ref_tree(self, k, proto=False):
        """the context node a symbolic reference stands for"""
        refs = self.m.refs
        if k < 0 or k >= len(refs): return None
        r = refs[k]
        if r[0] == 'local':
            sym = self.m.name_at(r[1])
            if sym is None:
                c = self.meta.ctx.get(r[1])
                return None
        elif r[0] == 'extern':
            sym = r[1]
        elif r[0] == 'objcproto':
            name = r[1]
            for p in ('_OBJC_PROTOCOL_$_', '__OBJC_PROTOCOL_$_', '_OBJC_LABEL_PROTOCOL_$_'):
                if name.startswith(p): name = name[len(p):]
            return N('Protocol', None, 0, [N('Module', '__C'), N('Identifier', name)])
        else:
            return None
        t = self.g.tree(sym)
        if not t or not t.c: return None
        d = t.c[0]
        # NominalTypeDescriptor / ProtocolDescriptor / OpaqueTypeDescriptor > Type > X
        if d.k in ('NominalTypeDescriptor', 'ProtocolDescriptor', 'TypeMetadata', 'FullTypeMetadata', 'TypeMetadataAccessFunction'):
            x = d.c[0]
            return x.c[0] if x.k == 'Type' else x
        if d.k == 'OpaqueTypeDescriptor':
            return d
        if d.k in ('Structure', 'Class', 'Enum', 'Protocol'):
            return d
        return None

    # ------------------------------------------------------------------------------------------ printing types
    def nominal_name(self, n, env, args=None):
        """qualified name of a nominal node; args: list of lists of printed generic arguments per level"""
        k = n.k
        if k == 'Type': return self.nominal_name(n.c[0], env, args)
        if k == 'TypeSymbolicReference':
            t = self.ref_tree(int(n.t))
            if t is None:
                self.problems['unresolved ref'] += 1
                return 'Any /*ref%s*/' % n.t
            return self.nominal_name(t, env, args)
        if k == 'ProtocolSymbolicReference':
            t = self.ref_tree(int(n.t), proto=True)
            if t is None: return 'Any /*pref*/'
            return self.nominal_name(t, env, args)
        if k == 'Module': return n.t
        if k == 'Extension': return self.nominal_name(n.c[1], env, args)
        if k.startswith('BoundGeneric'):
            base = n.c[0].c[0] if n.c[0].k == 'Type' else n.c[0]
            a = [self.ty(x, env) for x in n.c[1].c]
            return self.bound(base, a, env)
        par = n.c[0]
        nm = self.name_of(n)
        mod = par.k == 'Module' and par.t
        own = None
        if args:
            own = args[-1]; args = args[:-1]
        if mod in ('__C', '__C_Synthesized'):
            rel = [c for c in n.c if c.k == 'RelatedEntityDeclName']
            full = nm
            if rel: full = nm + '.' + {'e': 'Code'}.get(rel[0].t, rel[0].t)
            s = self.renames.get(full, full)
            if mod == '__C_Synthesized' and not rel: s = self.renames.get(nm, nm)
        elif mod:
            if mod == 'SwiftUI':
                s = 'SwiftUI.' + esc_member(self.emitted_name(n))
            else:
                s = mod + '.' + esc_member(nm)
        else:
            if par.k in ('Function', 'Getter', 'Variable', 'Allocator', 'ExplicitClosure', 'Subscript', 'Static', 'ImplicitClosure'):
                s = 'SwiftUI.' + self.emitted_name(n)
            else:
                ps = self.nominal_name(par, env, args)
                s = ps + '.' + esc_member(self.emitted_name(n) if self.key(par).startswith('SwiftUI') else nm)
        if own is not None and own:
            s += '<' + ', '.join(own) + '>'
        return s

    def emitted_name(self, n):
        """the name we give a SwiftUI type (private and local types get a unique name if needed)"""
        k = self.key(n)
        ti = self.types.get(k) or self.protocols.get(k)
        if ti is not None: return ti.ename
        return self.name_of(n)

    def levels(self, base):
        """generic parameter counts per nesting level of a nominal node (outermost first)"""
        k = self.key(base)
        ti = self.types.get(k)
        if ti is not None:
            return ti.level_counts
        name = self.nominal_name(base, Env())
        if name in EXTERN_GP: return [len(EXTERN_GP[name])]
        return None

    def bound(self, base, a, env):
        if base.k == 'TypeSymbolicReference':
            t = self.ref_tree(int(base.t))
            if t is None: return 'Any'
            base = t
        name = self.nominal_name(base, env)
        if name == 'Swift.Optional' and len(a) == 1:
            x = a[0]
            if re.search(r'[ &>]', x) or x.startswith('some ') or x.startswith('any '): x = '(' + x + ')'
            return x + '?'
        if name == 'Swift.Array' and len(a) == 1: return '[' + a[0] + ']'
        if name == 'Swift.Dictionary' and len(a) == 2: return '[' + a[0] + ': ' + a[1] + ']'
        lv = self.levels(base)
        if lv and len(lv) > 1 and sum(lv) == len(a):
            out = []; i = 0
            for c in lv:
                out.append(a[i:i + c]); i += c
            return self.nominal_name(base, env, out)
        return name + '<' + ', '.join(a) + '>'

    def ty(self, n, env, top=False):
        """print a Type node"""
        if n is None: return 'Any'
        k = n.k
        if k == 'Type': return self.ty(n.c[0], env, top)
        if k in ('Structure', 'Class', 'Enum', 'Protocol', 'TypeAlias', 'OtherNominalType', 'TypeSymbolicReference',
                 'ProtocolSymbolicReference'):
            s = self.nominal_name(n, env)
            if k == 'Protocol' or k == 'ProtocolSymbolicReference' or (k == 'TypeSymbolicReference' and self.is_protocol_ref(n)):
                return s
            return s
        if k.startswith('BoundGeneric'):
            return self.nominal_name(n, env)
        if k == 'Tuple':
            els = []
            for e in n.c:
                lab = e.first('TupleElementName')
                t = e.first('Type')
                s = self.ty(t, env)
                if e.first('VariadicMarker'): s += '...'
                els.append((lab.t + ': ' if lab else '') + s)
            if len(els) == 1 and not n.c[0].first('TupleElementName'): return '(' + els[0] + ')'
            return '(' + ', '.join(els) + ')'
        if k in ('FunctionType', 'NoEscapeFunctionType', 'AutoClosureType', 'EscapingAutoClosureType', 'ThinFunctionType',
                 'CFunctionPointer', 'ObjCBlock', 'EscapingObjCBlock', 'UncurriedFunctionType'):
            return self.fn_type(n, env)
        if k == 'Metatype':
            t = n.first('Type')
            s = self.ty(t, env)
            inner = t.c[0] if t.c else None
            if inner is not None and inner.k in ('ProtocolList', 'ProtocolListWithClass', 'ProtocolListWithAnyObject', 'Protocol',
                                                  'ProtocolSymbolicReference'):
                return '(' + s + ').Type'
            if re.search(r'[ &>]', s) and not s.endswith('>'): s = '(' + s + ')'
            return s + '.Type'
        if k == 'ExistentialMetatype':
            t = n.first('Type')
            s = self.ty(t, env)
            if s.startswith('any '): return 'any ' + s[4:] + '.Type' if '&' not in s else 'any (' + s[4:] + ').Type'
            return s + '.Type'
        if k == 'ProtocolList':
            tl = n.c[0] if n.c else None
            ps = [self.ty(x, env) for x in (tl.c if tl else [])]
            if not ps: return 'Any'
            return 'any ' + ' & '.join(ps)
        if k == 'ProtocolListWithClass':
            ps = [self.ty(x, env) for x in n.c[0].c[0].c] if n.c[0].c else []
            cl = self.ty(n.c[1], env)
            return ' & '.join([cl] + ps)
        if k == 'ProtocolListWithAnyObject':
            ps = [self.ty(x, env) for x in n.c[0].c[0].c] if n.c and n.c[0].c else []
            if not ps: return 'AnyObject'
            return 'any ' + ' & '.join(ps + ['AnyObject'])
        if k == 'DependentGenericParamType':
            return env.gp(idx(n.c[0]), idx(n.c[1]))
        if k == 'DependentMemberType':
            base = self.ty(n.c[0], env)
            ref = n.c[1]
            nm = ref.c[0].t if ref.c else ref.t
            return base + '.' + esc_member(nm)
        if k == 'OpaqueReturnType':
            par = n.first('OpaqueReturnTypeParent')
            if par is not None and par.t in self.opaque:
                return 'some ' + self.opaque[par.t]
            if env.opaque:
                return 'some ' + env.opaque
            self.problems['opaque without constraint'] += 1
            return 'some Any'
        if k == 'OpaqueType':
            self.problems['opaque type of another declaration'] += 1
            return 'Any /*opaque*/'
        if k == 'InOut': return 'inout ' + self.ty(n.c[0], env)
        if k == 'Shared': return '__shared ' + self.ty(n.c[0], env)
        if k == 'Owned': return '__owned ' + self.ty(n.c[0], env)
        if k == 'Isolated': return 'isolated ' + self.ty(n.c[0], env)
        if k == 'Sending': return 'sending ' + self.ty(n.c[0], env)
        if k == 'Weak': return self.ty(n.c[0], env)
        if k == 'Unowned': return self.ty(n.c[0], env)
        if k == 'Unmanaged': return self.ty(n.c[0], env)
        if k == 'DynamicSelf': return 'Self'
        if k == 'SugaredOptional': return self.ty(n.c[0], env) + '?'
        if k == 'SugaredArray': return '[' + self.ty(n.c[0], env) + ']'
        if k == 'SugaredDictionary': return '[' + self.ty(n.c[0], env) + ': ' + self.ty(n.c[1], env) + ']'
        if k == 'SugaredParen': return '(' + self.ty(n.c[0], env) + ')'
        if k == 'BuiltinTypeName': return 'Builtin.' + (n.t or '?')
        if k == 'PackExpansion': return 'repeat ' + self.ty(n.c[0], env)
        if k == 'Pack': return '(' + ', '.join(self.ty(x, env) for x in n.c) + ')'
        if k == 'ConstrainedExistential':
            base = self.ty(n.c[0], env)
            return base + '<...>'
        if k == 'ErrorType': return 'Any'
        self.problems['type kind ' + k] += 1
        return 'Any /*%s*/' % k

    def is_protocol_ref(self, n):
        return False

    def fn_type(self, n, env, params_only=False):
        k = n.k
        attrs = []
        effects = []
        args = None; ret = None
        for c in n.c:
            if c.k == 'ConcurrentFunctionType': attrs.append('@Sendable')
            elif c.k == 'GlobalActorFunctionType': attrs.append('@' + self.ty(c.c[0], env).replace('_Concurrency.', ''))
            elif c.k == 'IsolatedAnyFunctionType': attrs.append('@isolated(any)')
            elif c.k == 'AsyncAnnotation': effects.insert(0, 'async')
            elif c.k == 'ThrowsAnnotation': effects.append('throws')
            elif c.k == 'TypedThrowsAnnotation': effects.append('throws(%s)' % self.ty(c.c[0], env))
            elif c.k == 'ArgumentTuple': args = c
            elif c.k == 'ReturnType': ret = c
            elif c.k == 'SendingResultFunctionType': pass
            elif c.k == 'DifferentiableFunctionType': pass
            elif c.k == 'ClangType': pass
        if k == 'CFunctionPointer': attrs.insert(0, '@convention(c)')
        if k in ('ObjCBlock', 'EscapingObjCBlock'): attrs.insert(0, '@convention(block)')
        if k == 'ThinFunctionType': attrs.insert(0, '@convention(thin)')
        ps = self.params(args, env)
        s = '(' + ', '.join(p[1] for p in ps) + ')'
        if effects: s += ' ' + ' '.join(effects)
        s += ' -> ' + self.ty(ret.c[0] if ret else None, env)
        if k in ('AutoClosureType', 'EscapingAutoClosureType'): attrs.insert(0, '@autoclosure')
        return ' '.join(attrs + [s])

    def params(self, args, env, decl=False):
        """parameters of an ArgumentTuple: [(type node, printed type)]"""
        if args is None: return []
        t = args.c[0]
        inner = t.c[0] if t.k == 'Type' and t.c else t
        if inner.k == 'Tuple':
            out = []
            for e in inner.c:
                tt = e.first('Type')
                s = self.param_type(tt, env, decl)
                if e.first('VariadicMarker'): s += '...'
                out.append((tt, s))
            return out
        return [(t, self.param_type(t, env, decl))]

    def param_type(self, t, env, decl):
        inner = t.c[0] if t is not None and t.k == 'Type' and t.c else t
        if inner is None: return 'Any'
        pre = ''
        while inner.k in ('InOut', 'Shared', 'Owned', 'Isolated', 'Sending', 'NoDerivative', 'CompileTimeConst'):
            pre += {'InOut': 'inout ', 'Shared': '__shared ', 'Owned': '__owned ', 'Isolated': 'isolated ',
                    'Sending': 'sending ', 'NoDerivative': '', 'CompileTimeConst': '_const '}[inner.k]
            inner = inner.c[0].c[0] if inner.c and inner.c[0].k == 'Type' else inner.c[0]
        s = self.ty(inner, env)
        if decl and inner.k in ('FunctionType', 'EscapingAutoClosureType', 'EscapingObjCBlock', 'CFunctionPointer'):
            if inner.k != 'CFunctionPointer': s = '@escaping ' + s
        if inner.k == 'EscapingAutoClosureType' and not decl: pass
        return pre + s

    # ------------------------------------------------------------------------------------------ model
    def sig_reqs(self, sig, env):
        """printed requirements of a DependentGenericSignature node"""
        out = []
        for r in sig.c:
            if r.k == 'DependentGenericConformanceRequirement':
                out.append('%s: %s' % (self.ty(r.c[0], env), self.ty(r.c[1], env)))
            elif r.k == 'DependentGenericSameTypeRequirement':
                out.append('%s == %s' % (self.ty(r.c[0], env), self.ty(r.c[1], env)))
            elif r.k == 'DependentGenericLayoutRequirement':
                lay = r.c[1].t if len(r.c) > 1 else 'C'
                if lay in ('C', 'D', 'N', 'R'):
                    out.append('%s: AnyObject' % self.ty(r.c[0], env))
            elif r.k == 'DependentGenericSameShapeRequirement':
                pass
            elif r.k == 'DependentGenericInverseConformanceRequirement':
                out.append('%s: ~%s' % (self.ty(r.c[0], env), 'Copyable'))
        return out

    def sig_params(self, sig):
        """[(depth, count)] of new generic parameters and the set of pack parameters"""
        counts = [int(c.t) for c in sig.c if c.k == 'DependentGenericParamCount']
        packs = set()
        for c in sig.c:
            if c.k == 'DependentGenericParamPackMarker':
                t = c.c[0].c[0]
                packs.add((idx(t.c[0]), idx(t.c[1])))
        return counts, packs

    def proto_ref_name(self, ref):
        if ref is None: return 'Any'
        sym = self.m.ref_name(ref)
        t = self.g.tree(sym)
        if t and t.c:
            d = t.c[0]
            x = d.c[0] if d.c else None
            if x is not None and x.k == 'Type': x = x.c[0]
            if x is not None: return self.nominal_name(x, Env())
        return sym

    def meta_req(self, r, env):
        """printed requirement of the metadata (generic context)"""
        p = self.ty(self.g.mtype(r['param']), env)
        k = r['kind']
        if k == 0:
            if 'objc' in r:
                nm = r['objc']
                for pre in ('_OBJC_PROTOCOL_$_', '__OBJC_PROTOCOL_$_', '_OBJC_LABEL_PROTOCOL_$_'):
                    if nm.startswith(pre): nm = nm[len(pre):]
                return '%s: %s' % (p, nm)
            return '%s: %s' % (p, self.proto_ref_name(r['protocol']))
        if k == 1: return '%s == %s' % (p, self.ty(self.g.mtype(r['type']), env))
        if k == 2: return '%s: %s' % (p, self.ty(self.g.mtype(r['type']), env))
        if k == 0x1f: return '%s: AnyObject' % p
        return None

    def build(self):
        g = self.g; m = self.m; meta = self.meta
        # ---- nominal types
        for c in meta.types:
            if c.kind not in ('struct', 'enum', 'class'): continue
            t = g.tree(c.symbol) if c.symbol else None
            if not t or not t.c or not t.c[0].c:
                self.problems['type without symbol'] += 1; continue
            node = t.c[0].c[0]
            if node.k == 'Type': node = node.c[0]
            ti = TypeInfo(key=self.key(node), node=node, kind=c.kind, ctx=c, name=self.name_of(node),
                          exported=c.symbol in self.exports, disc=self.disc_of(node))
            ti.ename = ti.name
            self.types[ti.key] = ti
            self.type_by_addr[c.addr] = ti
        for c in meta.protocols:
            t = g.tree(c.symbol) if c.symbol else None
            if not t or not t.c or not t.c[0].c:
                self.problems['protocol without symbol'] += 1; continue
            node = t.c[0].c[0]
            if node.k == 'Type': node = node.c[0]
            ti = TypeInfo(key=self.key(node), node=node, kind='protocol', ctx=c, name=self.name_of(node),
                          exported=c.symbol in self.exports, disc=self.disc_of(node))
            ti.ename = ti.name
            self.protocols[ti.key] = ti
            self.types[ti.key] = ti
        # unique names for private and local types
        seen = collections.defaultdict(list)
        for ti in self.types.values():
            par = ti.node.c[0]
            pk = self.key(par) if par.k not in ('Function', 'Getter', 'Variable', 'Allocator', 'ExplicitClosure', 'Subscript',
                                                'Static', 'ImplicitClosure') else 'SwiftUI'
            ti.parent_key = pk
            seen[(pk, ti.name)].append(ti)
        for (pk, nm), lst in seen.items():
            if len(lst) > 1:
                for ti in lst:
                    if ti.disc: ti.ename = '%s_%s' % (ti.name, ti.disc.strip('_')[:6])
        # local types are declared at the top level of the module
        for ti in self.types.values():
            par = ti.node.c[0]
            ti.local = par.k in ('Function', 'Getter', 'Variable', 'Allocator', 'ExplicitClosure', 'Subscript', 'Static', 'ImplicitClosure')
            if ti.local:
                ti.parent_key = 'SwiftUI'
                ti.ename = '%s_L%s' % (ti.name, (ti.disc or 'L').strip('L_') or '0')
        # generic parameters per level
        def depths(ti, stack=()):
            if hasattr(ti, 'depths'): return ti.depths
            total = len(ti.ctx.generic['params']) if getattr(ti.ctx, 'generic', None) else 0
            par = self.types.get(ti.parent_key)
            if ti.kind == 'protocol':
                ti.depths = [1]; ti.level_counts = [0]; ti.total = 1; ti.own = 0; return ti.depths
            if par is not None and par.kind != 'protocol' and par not in stack:
                pd = depths(par, stack + (ti,))
                ptot = par.total; plev = par.level_counts
            else:
                pd = []; ptot = 0; plev = []
                # nested in an extension of a type of another module
                if ti.node.c[0].k in ('Structure', 'Enum', 'Class', 'Extension') and ti.parent_key not in self.types:
                    pass
            own = total - ptot
            ti.total = total; ti.own = own
            ti.depths = pd + ([own] if own > 0 else [])
            ti.level_counts = plev + [own] if (plev or own) else []
            return ti.depths
        for ti in self.types.values(): depths(ti)
        for ti in self.types.values():
            ti.gp = {}
            d = 0
            for d, cnt in enumerate(ti.depths):
                for i in range(cnt): ti.gp[(d, i)] = gp_name(d, i)
            ti.own_depth = len(ti.depths) - 1 if ti.own > 0 else None
        # nesting
        self.top = []
        for ti in self.types.values():
            par = self.types.get(ti.parent_key)
            if par is not None and not ti.local and par.kind != 'protocol':
                par.nested.append(ti)
            else:
                self.top.append(ti)
        # ---- opaque type constraints
        self.opaque = {}
        for o in meta.opaques:
            if not o.symbol: continue
            parent = o.symbol[3:-4] if o.symbol.startswith('_$s') else o.symbol
            gen = getattr(o, 'generic', None)
            if not gen: continue
            reqs = gen['requirements']
            # the opaque parameters are the deepest ones
            parsed = []
            maxd = -1
            for r in reqs:
                t = g.mtype(r['param'])
                dd = self.param_depth(t)
                parsed.append((r, t, dd))
                if dd is not None: maxd = max(maxd, dd)
            cons = []
            for r, t, dd in parsed:
                if dd != maxd or t is None or t.c[0].k != 'DependentGenericParamType': continue
                if r['kind'] == 0:
                    cons.append(self.proto_ref_name(r['protocol']) if 'objc' not in r else r['objc'])
                elif r['kind'] == 2:
                    cons.append(self.ty(g.mtype(r['type']), Env()))
                elif r['kind'] == 0x1f:
                    cons.append('AnyObject')
            self.opaque[parent] = ' & '.join(cons) if cons else 'Any'

    def param_depth(self, t):
        """depth of the generic parameter a requirement subject is rooted in"""
        while t is not None:
            if t.k == 'Type': t = t.c[0] if t.c else None; continue
            if t.k == 'DependentGenericParamType': return idx(t.c[0])
            if t.k == 'DependentMemberType': t = t.c[0]; continue
            return None
        return None

    # ------------------------------------------------------------------------------------------ members
    ACCESSORS = {'Getter': 'get', 'Setter': 'set', 'ModifyAccessor': 'modify', 'ReadAccessor': 'read', 'Modify2Accessor': 'modify',
                 'Read2Accessor': 'read', 'UnsafeAddressor': 'address', 'UnsafeMutableAddressor': 'mutableAddress',
                 'WillSet': 'willSet', 'DidSet': 'didSet', 'InitAccessor': 'init', 'GlobalGetter': 'get',
                 'OwningAddressor': 'address', 'OwningMutableAddressor': 'mutableAddress', 'NativeOwningAddressor': 'address',
                 'NativeOwningMutableAddressor': 'mutableAddress', 'NativePinningAddressor': 'address',
                 'NativePinningMutableAddressor': 'mutableAddress', 'MaterializeForSet': 'modify'}
    DECLS = ('Function', 'Allocator', 'Constructor', 'Variable', 'Subscript', 'Deallocator', 'Destructor', 'IVarDestroyer',
             'IsolatedDeallocator')

    def entity(self, g):
        """(role, entity node, static, extra) of a Global node, or None"""
        if not g.c: return None
        x = g.c[0]
        if not x.c: return None
        role = 'decl'
        extra = None
        if x.k in ('PropertyDescriptor', 'MethodDescriptor', 'DispatchThunk', 'ObjCAttribute', 'NonObjCAttribute', 'DynamicAttribute',
                   'DefaultArgumentInitializer', 'EnumCase', 'MethodLookupFunction'):
            role = x.k
            if x.k == 'DefaultArgumentInitializer': extra = int(x.c[1].t) if len(x.c) > 1 else 0
            x = x.c[0]
        elif x.k == 'ProtocolWitness':
            if len(x.c) < 2: return None
            role = 'witness'; extra = x.c[0]; x = x.c[1]
        static = False
        if x.k == 'Static' and x.c: static = True; x = x.c[0]
        acc = None
        if x.k in self.ACCESSORS and x.c:
            acc = self.ACCESSORS[x.k]; x = x.c[0]
        if x.k == 'Static' and x.c: static = True; x = x.c[0]
        if not x.c: return None
        if x.k not in self.DECLS: return None
        return role, x, static, acc, extra

    def collect(self):
        g = self.g
        self.ents = collections.OrderedDict()
        self.witness = []
        addr_of = {}
        for name, ntype, nsect, value in self.m.symbols:
            if nsect and not (ntype & 0xe0): addr_of.setdefault(name, value)
        for name, ntype, nsect, value in sorted(self.m.symbols, key=lambda s: s[3]):
            if not nsect or (ntype & 0xe0): continue
            t = g.tree(name)
            if t is None: continue
            e = self.entity(t)
            if e is None: continue
            role, x, static, acc, extra = e
            if role == 'witness':
                self.witness.append((extra, x, static, acc, name)); continue
            kind = 'Allocator' if x.k == 'Constructor' else x.k
            ek = (static, kind + ':' + ''.join(c.key() for c in x.c))
            ent = self.ents.get(ek)
            if ent is None:
                ent = dict(node=x, static=static, kind=kind, acc=set(), exp=set(), roles=set(), syms=[], defaults=set(),
                           constructor=False, allocator=False, addr=value)
                self.ents[ek] = ent
            exported = name in self.exports
            ent['roles'].add(role)
            ent['syms'].append(name)
            if role == 'decl':
                if acc: ent['acc'].add(acc)
                if exported: ent['exp'].add(acc or 'decl')
                if x.k == 'Constructor': ent['constructor'] = True
                if x.k == 'Allocator': ent['allocator'] = True
            elif role in ('PropertyDescriptor', 'MethodDescriptor', 'DispatchThunk', 'EnumCase'):
                if exported: ent['exp'].add(role)
                if acc: ent['acc'].add(acc)
            elif role == 'DefaultArgumentInitializer':
                ent['defaults'].add(extra)
            elif role == 'ObjCAttribute':
                ent['objc'] = True

    # ------------------------------------------------------------------------------------------ context of a member
    def ctx_of(self, node):
        """(type key or None, extension signature node or None, extension node or None) of a declaration's context"""
        c = node.c[0]
        if c.k == 'Extension':
            sig = c.c[2] if len(c.c) > 2 and c.c[2].k == 'DependentGenericSignature' else None
            return self.key(c.c[1]), sig, c
        if c.k == 'Module':
            return None, None, None
        return self.key(c), None, None

    # ------------------------------------------------------------------------------------------ declarations
    def labels(self, node):
        ll = node.first('LabelList')
        if ll is None: return None
        return [(c.t if c.k == 'Identifier' else '_') for c in ll.c]

    def split_type(self, node):
        """(signature node or None, function/plain type node) of an entity's Type child"""
        t = node.first('Type')
        if t is None: return None, None
        x = t.c[0]
        if x.k == 'DependentGenericType':
            return x.c[0], x.c[1]
        return None, t

    def generic_clause(self, sig, env, base_depth):
        """(<...> text, where-clause list, env with the new parameters)"""
        if sig is None: return '', [], env
        counts, packs = self.sig_params(sig)
        names = dict(env.names)
        gps = []
        for j, cnt in enumerate(counts):
            d = base_depth + j
            for i in range(cnt):
                nm = gp_name(d, i)
                names[(d, i)] = nm
                gps.append(('each ' + nm) if (d, i) in packs else nm)
        env2 = env.child(names=names)
        reqs = self.sig_reqs(sig, env2)
        return ('<' + ', '.join(gps) + '>') if gps else '', reqs, env2

    def param_list(self, fn, env, labels, decl=True, defaults=()):
        args = fn.first('ArgumentTuple') if fn is not None else None
        ps = self.params(args, env, decl=True)
        if labels is None: labels = ['_'] * len(ps)
        if len(labels) < len(ps): labels = labels + ['_'] * (len(ps) - len(labels))
        out = []
        for i, (t, s) in enumerate(ps):
            lab = labels[i]
            d = ' = ' + '_skeletonDefault()' if i in defaults else ''
            out.append('%s a%d: %s%s' % (esc(lab) if lab != '_' else '_', i, s, d))
        return '(' + ', '.join(out) + ')', ps

    def effects(self, fn):
        e = []
        if fn.first('AsyncAnnotation'): e.append('async')
        if fn.first('ThrowsAnnotation'): e.append('throws')
        t = fn.first('TypedThrowsAnnotation')
        if t: e.append('throws(%s)' % self.ty(t.c[0], Env()))
        return (' ' + ' '.join(e)) if e else ''

    def result(self, fn, env):
        r = fn.first('ReturnType') if fn is not None else None
        if r is None: return 'Swift.Void'
        return self.ty(r.c[0], env)

    def body_for(self, rtype):
        """a body that type-checks for a declared result type"""
        if rtype.startswith('some '):
            c = rtype[5:]
            first = c.split(' & ')[0]
            stub = self.fixes.get('opaque_stub', {}).get(first)
            if stub: return '{ %s }' % stub
            return '{ _skeleton() }'
        return '{ _skeleton() }'

    def access(self, ent, cinfo):
        if cinfo.get('in_protocol'): return ''
        if 'force_public' in cinfo: return 'public ' if cinfo['force_public'] else ''
        if ent['exp']:
            return 'open ' if cinfo.get('open') and not ent['static'] and ent['kind'] not in ('Allocator',) and cinfo.get('member_open', True) else 'public '
        return ''

    def member_decl(self, ent, cinfo):
        """lines declaring one entity"""
        n = ent['node']; k = ent['kind']
        env = cinfo['env']
        base_depth = cinfo['depth']
        static = ent['static']
        acc = self.access(ent, cinfo)
        mods = ''
        is_class = cinfo.get('kind') == 'class'
        if static:
            mods += ('class ' if is_class and ent.get('vtable') else 'static ')
        if is_class and ent.get('override'): mods = 'override ' + mods
        if is_class and ent.get('final') and not static and k not in ('Allocator',): mods = 'final ' + mods
        if ent.get('objc') and is_class and not ent.get('override'): mods = '@objc ' + mods
        if cinfo.get('in_protocol'): acc = ''
        body = not cinfo.get('in_protocol')
        if k == 'Function':
            name = self.name_of(n)
            op = n.first('InfixOperator', 'PrefixOperator', 'PostfixOperator')
            sig, fn = self.split_type(n)
            gc, reqs, env2 = self.generic_clause(sig, env, base_depth)
            fnt = fn.c[0] if fn is not None and fn.k == 'Type' else fn
            if fnt is None or not fnt.k.endswith('FunctionType'):
                return ['// unsupported function %s' % name]
            plist, ps = self.param_list(fnt, env2, self.labels(n), defaults=ent['defaults'])
            r = self.result(fnt, env2)
            if op is not None:
                fix = {'PrefixOperator': 'prefix ', 'PostfixOperator': 'postfix '}.get(op.k, '')
                dn = name + ' '
                mods = mods.replace('static ', 'static ' + fix) if static else fix + mods
            else:
                dn = esc(name)
            s = '%s%sfunc %s%s%s%s' % (acc, mods, dn, gc, plist, self.effects(fnt))
            if r not in ('Swift.Void', '()'): s += ' -> ' + r
            if reqs: s += ' where ' + ', '.join(reqs)
            if body: s += ' ' + self.body_for(r)
            return [s]
        if k == 'Allocator':
            sig, fn = self.split_type(n)
            gc, reqs, env2 = self.generic_clause(sig, env, base_depth)
            fnt = fn.c[0] if fn is not None and fn.k == 'Type' else fn
            if fnt is None: return ['// unsupported init']
            plist, ps = self.param_list(fnt, env2, self.labels(n), defaults=ent['defaults'])
            r = fnt.first('ReturnType')
            fail = ''
            rt = r.c[0].c[0] if r is not None and r.c and r.c[0].c else None
            if rt is not None and rt.k == 'BoundGenericEnum' and self.key(rt.c[0]) == 'Swift.Optional':
                inner = rt.c[1].c[0]
                if self.key(inner) == cinfo.get('self_key'): fail = '?'
            if is_class:
                if not ent['constructor'] and not ent.get('required') and cinfo.get('has_designated'):
                    mods = 'convenience ' + mods.replace('final ', '')
                if ent.get('required'): mods = 'required ' + mods
            mods = mods.replace('static ', '').replace('class ', '')
            s = '%s%sinit%s%s%s%s' % (acc, mods, fail, gc, plist, self.effects(fnt))
            if reqs: s += ' where ' + ', '.join(reqs)
            if body: s += ' { _skeleton() }'
            return [s]
        if k in ('Variable', 'Subscript'):
            accs = ent['acc']
            sig, t = self.split_type(n)
            gc, reqs, env2 = self.generic_clause(sig, env, base_depth)
            settable = bool(accs & {'set', 'modify', 'mutableAddress'})
            if k == 'Variable':
                name = self.name_of(n)
                if name.startswith('$'): return []
                ty = self.ty(t, env2)
                head = '%s%svar %s: %s' % (acc, mods, esc(name), ty)
                if ent.get('stored_static'):
                    kw = 'var' if settable else 'let'
                    return ['%s%s%s %s: %s = _skeleton()' % (acc, mods, kw, esc(name), ty)]
            else:
                fnt = t.c[0] if t is not None and t.k == 'Type' else t
                if fnt is None or not fnt.k.endswith('FunctionType'): return ['// unsupported subscript']
                plist, ps = self.param_list(fnt, env2, self.labels(n))
                ty = self.result(fnt, env2)
                head = '%s%ssubscript%s%s -> %s' % (acc, mods, gc, plist, ty)
                if reqs: head += ' where ' + ', '.join(reqs)
            if acc and settable and 'set' not in ent['exp'] and 'modify' not in ent['exp'] and not cinfo.get('in_protocol'):
                head = head.replace(acc, acc + 'internal(set) ', 1) if acc.strip() in ('public', 'open') else head
            if cinfo.get('in_protocol'):
                return [head + (' { get set }' if settable else ' { get }')]
            if ty.startswith('some '):
                return [head + ' ' + self.body_for(ty)]
            if settable:
                return [head + ' {', '    get { _skeleton() }', '    set { _skeleton() }', '}']
            return [head + ' { _skeleton() }']
        return []

    # ------------------------------------------------------------------------------------------ types
    def type_env(self, ti):
        return Env(ti.gp, proto_self=(ti.kind == 'protocol'))

    def own_reqs(self, ti, env):
        gen = getattr(ti.ctx, 'generic', None)
        if not gen: return []
        mine = [self.meta_req(r, env) for r in gen['requirements']]
        par = self.types.get(ti.parent_key)
        if par is not None and getattr(par.ctx, 'generic', None) and par.kind != 'protocol':
            pe = self.type_env(par)
            theirs = set(self.meta_req(r, pe) for r in par.ctx.generic['requirements'])
        else:
            theirs = set()
        return [r for r in mine if r and r not in theirs]

    def attach(self):
        """attach entities to their types / extension blocks"""
        self.blocks = collections.OrderedDict()   # (tkey, sigkey) -> dict(sig, ext, ents)
        self.globals = []
        self.reqs = collections.defaultdict(list)  # protocol key -> requirement entities
        for ek, ent in self.ents.items():
            n = ent['node']
            if n.k in ('Deallocator', 'Destructor', 'IVarDestroyer', 'IsolatedDeallocator'):
                tk = self.key(n.c[0])
                if tk in self.types: self.types[tk].has_deinit = True
                continue
            c = n.c[0]
            if c.k == 'Protocol':
                self.reqs[self.key(c)].append(ent); continue
            tk, sig, ext = self.ctx_of(n)
            if tk is None:
                if c.k == 'Module': self.globals.append(ent)
                continue
            if tk.startswith('local:'): continue
            sk = sig.key() if sig is not None else ''
            if ext is None and tk in self.types and self.types[tk].kind != 'protocol':
                self.types[tk].members.append(ent); continue
            b = self.blocks.get((tk, sk))
            if b is None:
                b = self.blocks[(tk, sk)] = dict(sig=sig, ext=ext, ents=[], key=tk)
            b['ents'].append(ent)

    def field_decls(self, ti, env, consumed):
        out = []
        f = ti.ctx.fields
        if not f: return out
        vars_by_name = {}
        for ent in ti.members:
            if ent['kind'] == 'Variable' and not ent['static']:
                vars_by_name[self.name_of(ent['node'])] = ent
        for r in f['records']:
            nm = r['name']
            t = self.g.mtype(r['type']) if r['type'] else None
            if ti.kind == 'enum':
                continue
            pre = ''
            tt = t.c[0] if t is not None and t.c else None
            if tt is not None and tt.k == 'Weak': pre = 'weak '
            elif tt is not None and tt.k == 'Unowned': pre = 'unowned '
            elif tt is not None and tt.k == 'Unmanaged': pre = 'unowned(unsafe) '
            ty = self.ty(t, env)
            ent = vars_by_name.get(nm)
            acc = ''
            if ent is not None:
                consumed.add(id(ent))
                if ent['exp']:
                    acc = 'public '
                    if r['var'] and not ({'set', 'modify'} & ent['exp']): acc = 'public internal(set) '
            kw = 'var' if r['var'] or pre else 'let'
            init = ''
            if ti.kind == 'class': init = ' = _skeletonValue()' if not pre else ''
            out.append('%s%s%s %s: %s%s' % (acc, pre, kw, esc(nm), ty, init))
        return out

    def case_decls(self, ti, env):
        out = []
        f = ti.ctx.fields
        if not f: return out
        for r in f['records']:
            nm = esc(r['name'])
            if r['type']:
                t = self.g.mtype(r['type'])
                s = self.ty(t, env)
                if not s.startswith('('): s = '(' + s + ')'
                out.append('%scase %s%s' % ('indirect ' if r['indirect'] else '', nm, s))
            else:
                out.append('case %s' % nm)
        return out

    def type_decl(self, ti, ind=''):
        env = self.type_env(ti)
        L = []
        acc = 'public ' if ti.exported else ''
        if ti.kind == 'protocol':
            return self.protocol_decl(ti, ind)
        gps = ''
        if ti.own > 0:
            d = ti.own_depth
            gps = '<' + ', '.join(gp_name(d, i) for i in range(ti.own)) + '>'
        inherit = []
        attrs = ''
        if ti.kind == 'class':
            sc = getattr(ti.ctx, 'superclass', None)
            if sc: inherit.append(self.ty(self.g.mtype(sc), env))
            if ti.exported and (ti.key in self.subclassed or any(e['exp'] & {'MethodDescriptor', 'DispatchThunk'} for e in ti.members)):
                acc = 'open '
            elif not (ti.ctx.ksf & (1 << 15)) and ti.key not in self.subclassed:
                attrs += 'final '
        if ti.kind == 'enum' and ti.exported and not any('EnumCase' in e['roles'] for e in ti.members) and ti.ctx.fields and ti.ctx.fields['records']:
            attrs = '@frozen ' + attrs
        reqs = self.own_reqs(ti, env)
        kw = {'struct': 'struct', 'enum': 'enum', 'class': 'class'}[ti.kind]
        head = '%s%s%s %s%s' % (attrs, acc, kw, esc(ti.ename), gps)
        if inherit: head += ': ' + ', '.join(inherit)
        if reqs: head += ' where ' + ', '.join(reqs)
        L.append(ind + head + ' {')
        consumed = set()
        for x in self.field_decls(ti, env, consumed): L.append(ind + '    ' + x)
        if ti.kind == 'enum':
            for x in self.case_decls(ti, env): L.append(ind + '    ' + x)
        cinfo = dict(env=env, depth=len(ti.depths), kind=ti.kind, self_key=ti.key, open=acc == 'open ',
                     has_designated=any(e['constructor'] for e in ti.members))
        for ent in ti.members:
            if id(ent) in consumed: continue
            if ti.kind == 'enum' and ent['kind'] == 'Function' and 'EnumCase' in ent['roles']: continue
            self.class_flags(ti, ent)
            for x in self.member_decl(ent, cinfo): L.append(ind + '    ' + x)
        if ti.kind == 'class' and getattr(ti, 'has_deinit', False):
            L.append(ind + '    deinit {}')
        for sub in ti.nested:
            L.extend(self.type_decl(sub, ind + '    '))
        L.append(ind + '}')
        return L

    def class_flags(self, ti, ent):
        if ti.kind != 'class': return
        vt = self.vtable_syms.get(ti.key, set())
        ov = self.override_syms.get(ti.key, set())
        syms = set(ent['syms'])
        ent['vtable'] = bool(syms & vt) or bool(ent['exp'] & {'MethodDescriptor', 'DispatchThunk'})
        ent['override'] = bool(syms & ov) or (ti.key + '|' + self.member_sig(ent)) in self.fixes.get('override', [])
        ent['final'] = not ent['vtable'] and not ent['override'] and not ent.get('objc') and bool(ti.ctx.ksf & (1 << 15))
        ent['required'] = (ti.key + '|' + self.member_sig(ent)) in self.fixes.get('required', [])

    def member_sig(self, ent):
        n = ent['node']
        nm = self.name_of(n) or ('init' if ent['kind'] == 'Allocator' else 'subscript')
        lab = self.labels(n) or []
        return '%s%s(%s)' % ('static ' if ent['static'] else '', nm, ':'.join(lab))

    def protocol_decl(self, ti, ind):
        env = Env({}, proto_self=True)
        c = ti.ctx
        acc = 'public ' if ti.exported else ''
        bases = []; where = []; assoc = collections.OrderedDict((a, []) for a in c.assoc_names)
        for r in c.signature:
            t = self.g.mtype(r['param'])
            tt = t.c[0] if t is not None and t.c else None
            s = self.meta_req(r, env)
            if s is None: continue
            if tt is not None and tt.k == 'DependentGenericParamType' and r['kind'] in (0, 2, 0x1f):
                bases.append(s.split(': ', 1)[1])
            elif tt is not None and tt.k == 'DependentMemberType' and tt.c[0].c[0].k == 'DependentGenericParamType' and r['kind'] in (0, 2, 0x1f):
                ref = tt.c[1]
                nm = ref.c[0].t if ref.c else ref.t
                if nm in assoc: assoc[nm].append(s.split(': ', 1)[1])
                else: where.append(s)
            else:
                where.append(s)
        head = '%sprotocol %s' % (acc, esc(ti.ename))
        if bases: head += ': ' + ', '.join(dict.fromkeys(bases))
        if where: head += ' where ' + ', '.join(where)
        L = [ind + head + ' {']
        for a, cons in assoc.items():
            L.append(ind + '    associatedtype %s%s' % (esc(a), (': ' + ' & '.join(dict.fromkeys(cons))) if cons else ''))
        cinfo = dict(env=env, depth=1, kind='protocol', in_protocol=True, self_key=ti.key)
        seen = set()
        for ent in self.reqs.get(ti.key, []):
            sg = self.member_sig(ent)
            if sg in seen: continue
            seen.add(sg)
            for x in self.member_decl(ent, cinfo): L.append(ind + '    ' + x)
        L.append(ind + '}')
        return L

    # ------------------------------------------------------------------------------------------ conformances
    def collect_classes(self):
        self.vtable_syms = {}; self.override_syms = {}; self.subclassed = set()
        for ti in self.types.values():
            if ti.kind != 'class': continue
            self.vtable_syms[ti.key] = set(v['impl_name'] for v in ti.ctx.vtable if v['impl_name'])
            self.override_syms[ti.key] = set(o['impl_name'] for o in ti.ctx.overrides if o['impl_name'])
            sc = getattr(ti.ctx, 'superclass', None)
            if sc:
                t = self.g.mtype(sc)
                if t is not None: self.subclassed.add(self.key(t))

    def collect_conformances(self):
        g = self.g
        assoc = collections.defaultdict(list)
        for a in self.meta.assoc:
            t = g.mtype(a['type']); p = g.mtype(a['protocol'])
            if t is None or p is None: continue
            assoc[(self.key(t), self.key(p))].append((a, t))
        self.confs = collections.defaultdict(list)
        self.alias_done = {}
        for cf in self.meta.conformances:
            t = g.tree(cf['symbol']) if cf['symbol'] else None
            if t is None or not t.c: self.problems['conformance without symbol'] += 1; continue
            pc = t.c[0].c[0]
            tyn = pc.c[0]; pn = pc.c[1]
            sig = None
            x = tyn.c[0]
            if x.k == 'DependentGenericType':
                sig = x.c[0]; tyn = x.c[1]
            tk = self.key(tyn); pk = self.key(pn)
            self.confs[tk].append(dict(tyn=tyn, pn=pn, sig=sig, pkey=pk, assoc=assoc.get((tk, pk), []),
                                       exported=cf['symbol'] in self.exports))

    def extern_env(self, node):
        name = self.nominal_name(node, Env())
        gps = EXTERN_GP.get(name, [])
        return Env({(0, i): nm for i, nm in enumerate(gps)}), (1 if gps else 0), name

    def conformance_decl(self, tk, cf):
        ti = self.types.get(tk)
        if ti is not None:
            env = self.type_env(ti)
            name = self.qualified(ti)
        else:
            base = cf['tyn'].c[0]
            if base.k.startswith('BoundGeneric'): base = base.c[0].c[0]
            env, _, name = self.extern_env(base)
        pname = self.ty(cf['pn'], Env())
        head = 'extension %s: %s' % (name, pname)
        if cf['sig'] is not None:
            reqs = self.sig_reqs(cf['sig'], env)
            if reqs: head += ' where ' + ', '.join(reqs)
        L = [head + ' {']
        pub = 'public ' if cf['exported'] and (ti is None or ti.exported) else ''
        nested = set(s.ename for s in ti.nested) if ti is not None else set()
        members = set(self.name_of(e['node']) for e in ti.members) if ti is not None else set()
        done = self.alias_done.setdefault(tk, set())
        for a, t in cf['assoc']:
            for nm, tn in a['records']:
                if nm in nested or nm in members or nm in done: continue
                tt = self.g.mtype(tn)
                s = self.ty(tt, env)
                if 'opaque' in s or 'some ' in s or '/*' in s: continue
                done.add(nm)
                L.append('    %stypealias %s = %s' % (pub, esc(nm), s))
        # requirements whose witness the binary has only inlined (no symbol of its own): declare them from the requirement
        if ti is not None and cf['pkey'] in self.protocols:
            have = set(self.member_sig(e) for e in ti.members)
            for (bk, sk), b in self.blocks.items():
                if bk == tk: have |= set(self.member_sig(e) for e in b['ents'])
            defaults = set()
            for (bk, sk), b in self.blocks.items():
                if bk == cf['pkey']: defaults |= set(self.member_sig(e) for e in b['ents'])
            env_r = Env({}, proto_self=True, class_self=name if ti.kind == 'class' else None)
            cinfo = dict(env=env_r, depth=1, kind=ti.kind, self_key=tk, force_public=bool(pub))
            seen = set()
            for ent in self.reqs.get(cf['pkey'], []):
                sg = self.member_sig(ent)
                if sg in have or sg in defaults or sg in seen: continue
                seen.add(sg)
                for x in self.member_decl(ent, cinfo): L.append('    ' + x)
        L.append('}')
        return L

    def qualified(self, ti):
        parts = [ti.ename]
        p = self.types.get(ti.parent_key)
        while p is not None and p.kind != 'protocol' and not ti.local:
            parts.insert(0, p.ename); p = self.types.get(p.parent_key)
        if ti.node.c[0].k == 'Extension' or (ti.parent_key not in self.types and ti.parent_key != 'SwiftUI' and not ti.local):
            par = ti.node.c[0]
            if par.k == 'Extension': par = par.c[1]
            return self.nominal_name(par, Env()) + '.' + '.'.join(esc_member(x) for x in parts)
        return 'SwiftUI.' + '.'.join(esc_member(x) for x in parts)

    # ------------------------------------------------------------------------------------------ blocks
    def block_decl(self, b):
        tk = b['key']
        ti = self.types.get(tk)
        ext = b['ext']
        if ti is not None:
            env = self.type_env(ti)
            name = self.qualified(ti)
            depth = len(ti.depths) if ti.kind != 'protocol' else 1
            kind = ti.kind
        else:
            base = ext.c[1] if ext is not None else None
            if base is None: return []
            env, depth, name = self.extern_env(base)
            kind = 'extern'
        head = 'extension %s' % name
        if b['sig'] is not None:
            reqs = self.sig_reqs(b['sig'], env)
            if reqs: head += ' where ' + ', '.join(reqs)
        cinfo = dict(env=env, depth=depth, kind=kind if kind != 'protocol' else 'protoext', self_key=tk)
        L = [head + ' {']
        for ent in b['ents']:
            if ti is not None: self.class_flags(ti, ent)
            for x in self.member_decl(ent, cinfo): L.append('    ' + x)
        L.append('}')
        return L

    # ------------------------------------------------------------------------------------------ files
    def run(self):
        self.build()
        self.collect()
        for ent in self.ents.values():
            if ent['kind'] == 'Variable' and ({'mutableAddress', 'address'} & ent['acc']) and (ent['static'] or ent['node'].c[0].k == 'Module'):
                ent['stored_static'] = True
        self.collect_classes()
        self.attach()
        self.collect_conformances()
        files = collections.defaultdict(list)
        owner_file = {}
        for ti in self.top:
            f = areas.file_for(ti)
            owner_file[ti.key] = f
            def mark(t, f):
                owner_file[t.key] = f
                for s in t.nested: mark(s, f)
            mark(ti, f)
            files[f].append('\n'.join(self.type_decl(ti)))
        for (tk, sk), b in self.blocks.items():
            if tk in self.types and self.types[tk].kind == 'protocol' and len(b['ents']) > 40:
                # large protocol extensions (View modifiers ...) are split by area of the member
                parts = collections.defaultdict(list)
                for ent in b['ents']:
                    parts[areas.member_file(self.types[tk], self.name_of(ent['node']) or 'init')].append(ent)
                for f, ents in parts.items():
                    files[f].append('\n'.join(self.block_decl(dict(b, ents=ents))))
                continue
            f = owner_file.get(tk) or areas.extern_file(tk)
            files[f].append('\n'.join(self.block_decl(b)))
        for tk, lst in self.confs.items():
            f = owner_file.get(tk) or areas.extern_file(tk)
            for cf in lst:
                files[f].append('\n'.join(self.conformance_decl(tk, cf)))
        gl = collections.defaultdict(list)
        for ent in self.globals:
            cinfo = dict(env=Env(), depth=0, kind='global')
            nm = self.name_of(ent['node']) or 'global'
            gl[areas.global_file(nm)].extend(self.member_decl(ent, cinfo))
        for f, lines in gl.items(): files[f].append('\n'.join(lines))
        files['Support/Skeleton.swift'].append(SKELETON)
        self.write(files)
        print('problems:', dict(self.problems), file=sys.stderr)

    def write(self, files):
        out = self.g.out
        # file names must be unique in a module (they make the private discriminators)
        base = collections.Counter(os.path.basename(f) for f in files)
        for f in list(files):
            b = os.path.basename(f)
            if base[b] > 1:
                d = os.path.dirname(f)
                files[os.path.join(d, d.split('/')[-1] + '_' + b)] += files.pop(f)
        for f, chunks in sorted(files.items()):
            p = os.path.join(out, f)
            os.makedirs(os.path.dirname(p), exist_ok=True)
            desc = areas.describe(f)
            with open(p, 'w') as fh:
                fh.write(HEADER % desc)
                fh.write('\n')
                for i in IMPORTS: fh.write('import %s\n' % i)
                fh.write('\n')
                fh.write('\n\n'.join(chunks))
                fh.write('\n')
        print('%d files written to %s' % (len(files), out), file=sys.stderr)


SKELETON = '''// Bodies not written yet end here.
@inline(never)
internal func _skeleton(_ function: StaticString = #function, file: StaticString = #fileID, line: UInt = #line) -> Never {
    fatalError("SwiftUI: \\(function) is not implemented yet", file: file, line: line)
}

/// Initial value of a stored property whose type has no value we can name yet.
internal func _skeletonValue<T>(_ function: StaticString = #function) -> T { _skeleton(function) }

/// Default argument not recovered yet.
internal func _skeletonDefault<T>(_ function: StaticString = #function) -> T { _skeleton(function) }'''

# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Which area (folder) a declaration of the skeleton belongs to; first matching rule wins."""
import re

RULES = [
    ('AppKitBridging', r'^NS|Hosting|Representable|AppKit|PlatformView|Responder|Coordinator|Panel|ControlTarget|MenuActionTarget|AppMenu'),
    ('Graph', r'Graph|^_?View(Inputs|Outputs|List|Traits?|Phase|Identity|Origin|Dimensions|Proxy|Test)|Attribute|DynamicProperty|Update|Rule|^_?Body|PropertyList|Signal|Subgraph'),
    ('Environment', r'Environment|EnvironmentKey|Preference|Trait|ContainerValue|Defaults?Key|ColorScheme|ControlSize|LayoutDirection|DynamicType|Locale'),
    ('State', r'State|Binding|Observ|Bindable|AppStorage|SceneStorage|FocusState|Namespace|StateObject|Published|Location|Storage'),
    ('Animation', r'Animat|Transaction|Spring|Keyframe|Transition|PhaseAnimator|UnitCurve|Timing|Interpolat|Velocity|ContentTransition'),
    ('Gestures', r'Gesture|Tap|Drag|Magnif|Rotat|LongPress|Hover|Pointer|Cursor|Drop|Draggable|Swipe|Pan'),
    ('Layout', r'Layout|Stack|Grid|Spacer|Frame|Padding|Geometry|Alignment|Edge|Inset|Offset|Position|Safe|Spacing|Divider|Proposal|Anchor|Coordinate|Overlay|Background|Aspect|Flexible|Fixed|Containe?r?Relative|Size|Axis|ScrollTarget'),
    ('Text', r'Text|Font|Attributed|Label|Inflection|Localiz|Format|Markdown|Kern|Tracking|Baseline|Truncat|Line(Limit|Spacing)|Typesetting'),
    ('Images', r'Image|Symbol|Icon|AsyncImage|Resizable|Interpolation'),
    ('Drawing', r'Shape|Path|Color|Gradient|Canvas|GraphicsContext|Material|Shader|Shadow|Blur|Blend|Opacity|Mask|Clip|Fill|Stroke|Rectangle|Circle|Ellipse|Capsule|Effect|Filter|Brightness|Contrast|Saturation|Hue|Visual|Mesh|Style|Tint|Foreground|ImageRender|Render|Drawing|Glass'),
    ('Navigation', r'Navigation|Tab|Split|Sidebar|Inspector|Sheet|Popover|Alert|Dialog|Presentation|Presented|Present|Window|Menu|Toolbar|Command|Search|Context'),
    ('Controls', r'Button|Toggle|Slider|Stepper|Picker|DatePicker|ColorPicker|TextField|SecureField|TextEditor|Link|Gauge|Progress|Control|Form|Section|Disclosure|Group|Share|Paste|Rename|Keyboard|Focus|Submit|Help|Badge|Selection|Label'),
    ('Lists', r'List|Table|Outline|ForEach|Lazy|Scroll|Row|Column|Refresh|Swipe|Move|Delete|Edit|Dynamic'),
    ('Scenes', r'Scene|App\b|^App|WindowGroup|Settings|DocumentGroup|MenuBarExtra|Widget|Launch|OpenURL|OpenWindow|Dismiss|Commands|FileDocument|Document|Import|Export'),
    ('Accessibility', r'Accessib'),
    ('Previews', r'Preview'),
    ('Views', r'View|Modifier|Content|Tuple|Empty|AnyView|Conditional|Optional|ForEach|Group|Never'),
]


def area(name):
    for a, rx in RULES:
        if re.search(rx, name): return a
    return 'Misc'


def file_for(ti):
    name = ti.ename
    return '%s/%s.swift' % (area(name), re.sub(r'[^A-Za-z0-9_+]', '_', name))


def member_file(ti, member):
    a = area(member[0].upper() + member[1:]) if member else 'Misc'
    return '%s/%s+%s.swift' % (a, ti.ename, a)


def extern_file(tk):
    n = tk.split('.')
    return 'Extensions/%s+SwiftUI.swift' % re.sub(r'[^A-Za-z0-9_+]', '_', '.'.join(n[-2:]) if len(n) > 1 else tk)


def global_file(name):
    a = area(name[0].upper() + name[1:] if name else 'Misc')
    return '%s/%sGlobals.swift' % (a, a)


def describe(f):
    a, b = f.split('/', 1)
    b = b[:-6]
    if a == 'Support': return 'Helpers of the reconstruction (stub bodies).'
    if a == 'Extensions': return 'What SwiftUI adds to %s.' % b.replace('+SwiftUI', '')
    if '+' in b: return '%s: members of the %s area.' % (b.split('+')[0], a)
    if b.endswith('Globals'): return 'Global functions and variables of the %s area.' % a
    return '%s (%s).' % (b, a)

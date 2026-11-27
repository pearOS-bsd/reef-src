// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
// Test-only entry compiled into the module (not part of Sources) by tools/selftest.sh: lays out a few elements and
// prints their frames, reads the environment and a State.
import Foundation
import CoreGraphics
import AppKit

@_cdecl("swiftui_selftest")
public func swiftui_selftest() {
    let a = SwiftUI.FrameElement(SwiftUI.Element(), width: 50, height: 20, alignment: .center)
    let s = SwiftUI.SpacerElement(minLength: nil)
    let b = SwiftUI.FrameElement(SwiftUI.Element(), width: 30, height: 40, alignment: .center)
    let h = SwiftUI.StackElement(axis: .horizontal, alignment: SwiftUI.VerticalAlignment.center.key, spacing: nil, items: [a, s, b])
    print("hstack measure", h.sizeThatFits(SwiftUI.ProposedViewSize(width: 300, height: 100)))
    h.place(in: CGRect(x: 0, y: 0, width: 300, height: 100))
    for e in [a, s, b] { print(" ", e.frame) }
    let p = SwiftUI.PaddingElement(SwiftUI.FrameElement(SwiftUI.Element(), width: 10, height: 10, alignment: .center), insets: SwiftUI.EdgeInsets(_all: 5))
    let v = SwiftUI.StackElement(axis: .vertical, alignment: SwiftUI.HorizontalAlignment.leading.key, spacing: 4, items: [p, SwiftUI.FrameElement(SwiftUI.Element(), width: 40, height: 10, alignment: .center)])
    print("vstack measure", v.sizeThatFits(.unspecified))
    v.place(in: CGRect(x: 10, y: 10, width: 100, height: 100))
    for e in v.items { print(" ", e.frame) }
    let f = SwiftUI.FlexFrameElement(SwiftUI.FrameElement(SwiftUI.Element(), width: 20, height: 20, alignment: .center),
        layout: SwiftUI._FlexFrameLayout(minWidth: nil, idealWidth: nil, maxWidth: .infinity, minHeight: nil, idealHeight: nil, maxHeight: nil, alignment: .center))
    print("flex inf", f.sizeThatFits(SwiftUI.ProposedViewSize(width: .infinity, height: 50)), "nil", f.sizeThatFits(.unspecified))
    var env = SwiftUI.EnvironmentValues()
    print("env", env.isEnabled, env.displayScale, env.lineLimit as Any, env.colorScheme)
    env.isEnabled = false
    print("env set", env.isEnabled)
    let st = SwiftUI.State(wrappedValue: 3)
    print("state", st.wrappedValue, st.projectedValue.wrappedValue)
    selftestBuild()
}

/// a primitive view of the test: one fixed-size element
struct SelfTestLeaf: SwiftUI.View, SwiftUI.PrimitiveView {
    typealias Body = Never
    var body: Never { fatalError() }
    let width: CGFloat
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        [SwiftUI.FrameElement(SwiftUI.Element(), width: width, height: 10, alignment: .center)]
    }
}

/// a view with state whose body is a leaf as wide as the state
struct SelfTestCounter: SwiftUI.View {
    @SwiftUI.State var count = 7
    var body: SelfTestLeaf { SelfTestLeaf(width: CGFloat(count)) }
}

/// ViewBuild: a body built under its node, reused in the next generation, built again after a State change
func selftestBuild() {
    let host = SwiftUI.ViewHost(rootType: SelfTestCounter.self)
    func context() -> SwiftUI.BuildContext {
        SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(), canvas: nil,
            modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    }
    host.generation = 1
    let first = SwiftUI.ViewBuild.build(SelfTestCounter(), context())
    let node = host.root.children.values.first!
    print("build", first.count, first[0].sizeThatFits(.unspecified), "runs", node.bodyRuns, "entries",
          SwiftUI.DynamicPropertyLayout.entries(of: SelfTestCounter.self).count)
    host.generation = 2
    let second = SwiftUI.ViewBuild.build(SelfTestCounter(), context())
    print("reuse", second[0] === first[0], "runs", node.bodyRuns)
    (node.storage.values.first as! SwiftUI.StoredLocation<Int>).set(12, transaction: SwiftUI.Transaction())
    print("dirty", node.isDirty, node.needsBuild, host.needsUpdate)
    host.generation = 3
    let third = SwiftUI.ViewBuild.build(SelfTestCounter(), context())
    print("rebuild", third[0] === first[0], third[0].sizeThatFits(.unspecified), "runs", node.bodyRuns)
    selftestStacks(host)
    selftestFonts()
    selftestTextBuilder()
    selftestTextLayout(host)
    selftestLocalized()
    selftestForEach(host)
    selftestTextEnvironment(host)
    selftestColors()
    selftestEnvironmentModifiers(host)
}

/// a leaf as wide as 10, or 20 when enabled
struct SelfTestEnv: SwiftUI.View, SwiftUI.PrimitiveView {
    typealias Body = Never
    var body: Never { fatalError() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        [SwiftUI.FrameElement(SwiftUI.Element(), width: a0.environment.isEnabled ? 20 : 10, height: 1, alignment: .center)]
    }
}

/// a body of stacks, a spacer, a condition and a tuple
struct SelfTestStacks: SwiftUI.View {
    var wide: Bool
    @ViewBuilder var body: some SwiftUI.View {
        SwiftUI.VStack(alignment: .leading, spacing: 4) {
            SwiftUI.HStack(alignment: .center, spacing: nil) {
                SelfTestLeaf(width: 30)
                SwiftUI.Spacer(minLength: nil)
                SelfTestLeaf(width: 20)
            }
            if wide { SelfTestLeaf(width: 100) } else { SelfTestLeaf(width: 10) }
            SwiftUI.ZStack(alignment: .center) { SelfTestLeaf(width: 5) }
                .padding(3)
                .frame(width: 60, height: nil, alignment: .trailing)
                .environment(\.isEnabled, false)
            SwiftUI.Group { SelfTestEnv().disabled(true).font(nil) }
        }
        .padding(.horizontal, nil)
    }
}

func selftestStacks(_ host: SwiftUI.ViewHost) {
    let context = SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(),
        canvas: nil, modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    for wide in [false, true] {
        let elements = SwiftUI.ViewBuild.build(SelfTestStacks(wide: wide), context)
        let root = elements[0]
        print("stacks", elements.count, type(of: root), root.sizeThatFits(SwiftUI.ProposedViewSize(width: 200, height: 100)))
        root.place(in: CGRect(x: 0, y: 0, width: 200, height: 100))
        for child in root.children {
            print(" ", type(of: child), child.frame, child.children.map { $0.frame })
        }
    }
}

func selftestFonts() {
    func traits(_ font: SwiftUI.Font) -> SwiftUI.FontTraits {
        var t = SwiftUI.FontTraits(size: 13, weight: nil, italic: false, design: nil, monospaced: false, monospacedDigit: false,
                                   name: nil, width: nil, leading: nil)
        font.provider.apply(to: &t)
        return t
    }
    for (label, font) in [("body", SwiftUI.Font.body), ("headline", .headline), ("largeTitle.italic", .largeTitle.italic()),
                          ("system20.weight", .system(size: 20, weight: nil, design: .monospaced).weight(.light)),
                          ("custom", .custom("Menlo", size: 0))] {
        let t = traits(font)
        print("font", label, t.size, t.weight?.value as Any, t.italic, t.design as Any, t.name as Any)
    }
    print("font equal", SwiftUI.Font.body == .body, SwiftUI.Font.body == .headline, SwiftUI.Font.body.bold() == .body.bold())
    print("line", [10, 13, 17, 20, 26].map { SwiftUI.FontResolver.lineHeight($0) }, [10, 15, 20].map { SwiftUI.FontResolver.lineAdvance($0) })
    let ns = SwiftUI.FontResolver.nsFont(traits(.headline))
    print("nsfont", ns.pointSize, ns.fontDescriptor.symbolicTraits.contains(.bold))
}

func selftestTextBuilder() {
    var env = SwiftUI.EnvironmentValues()
    let text = SwiftUI.Text(verbatim: "Hello, ") + SwiftUI.Text("world" as Swift.Substring)
    let (string, style) = SwiftUI.TextBuilder.string(text, env)
    let font = string.attribute(.font, at: 0, effectiveRange: nil) as? NSFont
    let paragraph = string.attribute(.paragraphStyle, at: 0, effectiveRange: nil) as? NSParagraphStyle
    print("text", string.string, string.length, font?.pointSize as Any, style.traits.size, paragraph?.minimumLineHeight as Any,
          paragraph?.alignment.rawValue as Any, paragraph?.lineBreakMode.rawValue as Any)
    if case .color(let c) = style.paint { print("text paint", c.opacity > 0) } else { print("text paint none") }
    env.textCase = .uppercase
    env.multilineTextAlignment = .center
    let (upper, _) = SwiftUI.TextBuilder.string(SwiftUI.Text(verbatim: "ab") + SwiftUI.Text(verbatim: "c"), env)
    print("text case", upper.string, (upper.attribute(.paragraphStyle, at: 0, effectiveRange: nil) as? NSParagraphStyle)?.alignment.rawValue as Any)
}

struct SelfTestText: SwiftUI.View {
    var body: some SwiftUI.View {
        SwiftUI.VStack(alignment: .leading, spacing: nil) {
            SwiftUI.Text(verbatim: "Hello")
            SwiftUI.Text(verbatim: "A longer line of text that has to wrap")
        }
    }
}

func selftestTextLayout(_ host: SwiftUI.ViewHost) {
    let context = SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(),
        canvas: nil, modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    let root = SwiftUI.ViewBuild.build(SelfTestText(), context)[0]
    for width in [nil, 100 as CoreGraphics.CGFloat] {
        let size = root.sizeThatFits(SwiftUI.ProposedViewSize(width: width, height: nil))
        root.place(in: CGRect(origin: .zero, size: size))
        print("text layout", width as Any, size.height, root.children.map { $0.frame.height }, root.children.map { $0.frame.minY })
    }
    let text = root.children[0]
    print("text baseline", text.explicitAlignment(SwiftUI.VerticalAlignment.firstTextBaseline.key, size: text.frame.size) as Any,
          text.spacingKind)
    // drawing: a frame narrower than the string truncates, a wide one does not; drawsSame against itself and another text
    if let t = text as? SwiftUI.TextElement {
        let env = SwiftUI.EnvironmentValues()
        let other = SwiftUI.TextElement(text: SwiftUI.Text(verbatim: "Other"), environment: env)
        let same = SwiftUI.TextElement(text: SwiftUI.Text(verbatim: "Hello"), environment: env)
        let again = SwiftUI.TextElement(text: SwiftUI.Text(verbatim: "Hello"), environment: env)
        for width in [10 as CoreGraphics.CGFloat, 400] {
            t.place(in: CGRect(x: 5, y: 7, width: width, height: 16))
            let d = t.drawing()
            print("text drawing", d.rect, d.options.rawValue, d.clips)
        }
        print("text same", t.drawsSame(as: t), same.drawsSame(as: again), same.drawsSame(as: other),
              SwiftUI.Text(verbatim: "a") == SwiftUI.Text(verbatim: "a"), SwiftUI.Text(verbatim: "a").bold() == SwiftUI.Text(verbatim: "a"),
              SwiftUI.Text(verbatim: "a").foregroundColor(.red) == SwiftUI.Text(verbatim: "a").foregroundColor(.red))
    }
}

func selftestLocalized() {
    func plain(_ text: SwiftUI.Text) -> Swift.String {
        SwiftUI.TextBuilder.string(text, SwiftUI.EnvironmentValues()).0.string
    }
    let count = 3
    let name = "Ana"
    let key: SwiftUI.LocalizedStringKey = "Hello \(name), \(count) items at 100%"
    print("localized", key.key, key.hasFormatting, key.arguments.count)
    func text(_ k: SwiftUI.LocalizedStringKey) -> SwiftUI.Text { SwiftUI.Text(k, tableName: nil, bundle: nil, comment: nil) }
    print("localized text", plain(text("Plain key")), plain(text(key)), plain(text("\(2.5) x")))
    if case .anyTextStorage(let storage) = text("Some **bold** word").storage {
        print("markdown", type(of: storage), storage.plainString)
    }
    // runs of an attributed text: strong emphasis is weight 0.4, code is monospaced, emphasis italic
    let runs = SwiftUI.TextBuilder.string(text("A **b** `c` *d*"), SwiftUI.EnvironmentValues()).0
    var described: [Swift.String] = []
    runs.enumerateAttribute(.font, in: NSRange(location: 0, length: runs.length), options: []) { value, range, _ in
        let f = value as? NSFont
        let traits = f.map { NSFontManager.shared.traits(of: $0) } ?? []
        described.append("\((runs.string as NSString).substring(with: range)):\(traits.contains(.boldFontMask) ? "b" : "")\(traits.contains(.italicFontMask) ? "i" : "")\(f?.isFixedPitch == true ? "m" : "")")
    }
    print("attributed runs", described)
    let styled = SwiftUI.Text(verbatim: "B").bold().italic().kerning(1).tracking(0.5).underline(true, color: nil)
        .font(.title).textScale(.secondary, isEnabled: true)
    let (string, style) = SwiftUI.TextBuilder.string(styled, SwiftUI.EnvironmentValues())
    let font = string.attribute(.font, at: 0, effectiveRange: nil) as? NSFont
    print("modifiers", style.traits.size, style.traits.weight?.value as Any, style.traits.italic, style.kerning, style.tracking,
          font?.pointSize as Any, string.attribute(.kern, at: 0, effectiveRange: nil) as Any,
          string.attribute(.underlineStyle, at: 0, effectiveRange: nil) as Any)
}

struct SelfTestItem: Swift.Identifiable { let id: Swift.String; let width: CoreGraphics.CGFloat }

struct SelfTestForEach: SwiftUI.View {
    var body: some SwiftUI.View {
        SwiftUI.VStack(alignment: .leading, spacing: 2) {
            SwiftUI.ForEach(0..<3) { SelfTestLeaf(width: CoreGraphics.CGFloat(10 * ($0 + 1))) }
            SwiftUI.ForEach([SelfTestItem(id: "a", width: 7), SelfTestItem(id: "b", width: 9)]) { SelfTestLeaf(width: $0.width) }
        }
    }
}

func selftestForEach(_ host: SwiftUI.ViewHost) {
    let context = SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(),
        canvas: nil, modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    let root = SwiftUI.ViewBuild.build(SelfTestForEach(), context)[0]
    let size = root.sizeThatFits(SwiftUI.ProposedViewSize(width: nil, height: nil))
    root.place(in: CGRect(origin: .zero, size: size))
    print("foreach", size, root.children.map { $0.frame.width }, root.children.map { $0.explicitID.map { "\($0.base)" } ?? "-" })
}

struct SelfTestTextEnvironment: SwiftUI.View {
    var body: some SwiftUI.View {
        SwiftUI.Text(verbatim: "A longer line of text that has to wrap")
            .lineLimit(1).truncationMode(.middle).multilineTextAlignment(.trailing).lineSpacing(4).font(.headline)
    }
}

func selftestTextEnvironment(_ host: SwiftUI.ViewHost) {
    let context = SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(),
        canvas: nil, modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    var element = SwiftUI.ViewBuild.build(SelfTestTextEnvironment(), context)[0]
    while !(element is SwiftUI.TextElement), let child = element.children.first { element = child }
    guard let text = element as? SwiftUI.TextElement else { print("text environment: no text"); return }
    let paragraph = text.string.attribute(.paragraphStyle, at: 0, effectiveRange: nil) as? NSParagraphStyle
    print("text environment", text.lineLimit as Any, text.lineSpacing, paragraph?.lineBreakMode.rawValue as Any,
          paragraph?.alignment.rawValue as Any, text.style.traits.weight?.value as Any,
          text.measure(SwiftUI.ProposedViewSize(width: 100, height: nil)))
    var dark = SwiftUI.EnvironmentValues()
    dark[SwiftUI.ColorSchemeKey.self] = .dark
    for (label, env) in [("light", SwiftUI.EnvironmentValues()), ("dark", dark)] {
        let (_, style) = SwiftUI.TextBuilder.string(SwiftUI.Text(verbatim: "c").foregroundColor(.red), env)
        if case .color(let c) = style.paint {
            print("color", label, (c.red * 255).rounded(), (c.green * 255).rounded(), (c.blue * 255).rounded(), c.opacity)
        }
    }
    let white = SwiftUI.Color.white.provider.resolve(in: SwiftUI.EnvironmentValues())
    print("color white", white.linearRed, white.opacity)
}

/// colours: equality and hashing of the boxes, stored statics, opacity, mixing, Resolved round trips
func selftestColors() {
    let env = SwiftUI.EnvironmentValues()
    func rgb(_ c: SwiftUI.Color) -> String {
        let r = c.resolve(in: env)
        return "\((r.red * 255).rounded()),\((r.green * 255).rounded()),\((r.blue * 255).rounded()),\((r.opacity * 100).rounded() / 100)"
    }
    print("color eq", SwiftUI.Color.red == SwiftUI.Color.red, SwiftUI.Color.red == SwiftUI.Color.blue,
          SwiftUI.Color.red.provider === SwiftUI.Color.red.provider,
          SwiftUI.Color(red: 1, green: 0.5, blue: 0, opacity: 1) == SwiftUI.Color(red: 1, green: 0.5, blue: 0, opacity: 1),
          SwiftUI.Color.red.opacity(0.5) == SwiftUI.Color.red.opacity(0.5), SwiftUI.Color.red.hashValue == SwiftUI.Color.red.hashValue,
          SwiftUI.Color("a", bundle: nil) == SwiftUI.Color("a", bundle: nil), SwiftUI.Color.accentColor == SwiftUI.Color.accentColor)
    print("color rgb", rgb(SwiftUI.Color(red: 1, green: 0.5, blue: 0, opacity: 1)), rgb(SwiftUI.Color(white: 0.5, opacity: 0.25)),
          rgb(SwiftUI.Color.red.opacity(0.5)), rgb(SwiftUI.Color(SwiftUI.Color.Resolved(colorSpace: .sRGB, red: 0.2, green: 0.4, blue: 0.6, opacity: 1))))
    print("color mix", rgb(SwiftUI.Color.black.mix(with: .white, by: 0.5, in: .device)), rgb(SwiftUI.Color.black.mix(with: .white, by: 2, in: .device)),
          rgb(SwiftUI.Color.black.mix(with: .white, by: 0.5, in: .perceptual)), rgb(SwiftUI.Color(provider: SwiftUI.MixedColorBox(base: SwiftUI.Color.black.provider, white: 0.9))))
    let resolved = SwiftUI.Color.Resolved(colorSpace: .sRGB, red: 0.5, green: 0.5, blue: 0.5, opacity: 1)
    print("color resolved", resolved == resolved.resolve(in: env), (resolved.cgColor.components ?? []).map { ($0 * 1000).rounded() / 1000 })
}

/// a leaf that keeps the environment it was built in
var selftestSeenEnvironment = SwiftUI.EnvironmentValues()
struct SelfTestEnvironmentProbe: SwiftUI.View, SwiftUI.PrimitiveView {
    typealias Body = Never
    var body: Never { fatalError() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        selftestSeenEnvironment = a0.environment
        return [SwiftUI.Element()]
    }
}

struct SelfTestEnvironmentModifiers: SwiftUI.View {
    var body: some SwiftUI.View {
        SelfTestEnvironmentProbe()
            .kerning(1.5).tracking(2).baselineOffset(-3).bold(true).italic(true).monospacedDigit().labelsHidden()
            .lineLimit(2...4).labelsVisibility(.visible).monospaced(true).scrollIndicators(.never, axes: .vertical)
            .contentMargins([.top, .trailing], 7, for: .automatic).defaultScrollAnchor(.center)
            .defaultScrollAnchor(.bottom, for: .sizeChanges).scrollEdgeEffectStyle(.hard, for: [.top, .bottom])
            .scrollEdgeEffectStyle(nil, for: .top).tableColumnHeaders(.hidden).keyboardShortcut("k", modifiers: .command)
            .underline(true, pattern: .solid, color: .red).textScale(.secondary, isEnabled: true)
            .fileDialogMessage(SwiftUI.Text(verbatim: "message")).onSubmit(of: .text) {}.onSubmit(of: .text) {}
            .tabViewSidebarFooter { SwiftUI.Text(verbatim: "footer") }
    }
}

func selftestEnvironmentModifiers(_ host: SwiftUI.ViewHost) {
    let context = SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(),
        canvas: nil, modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    _ = SwiftUI.ViewBuild.build(SelfTestEnvironmentModifiers(), context)
    let e = selftestSeenEnvironment
    print("env text", e[SwiftUI.TextKerningKey.self], e[SwiftUI.TextTrackingKey.self], e[SwiftUI.TextBaselineOffsetKey.self],
          e.textBold, e.textItalic, e.textMonospacedDigit, e[SwiftUI.TextScaleKey.self] as Any,
          e[SwiftUI.TextUnderlineKey.self]?.isActive as Any, e[SwiftUI.FontKey.self] != nil)
    print("env limits", e[SwiftUI.LineLimitKey.self] as Any, e.lineLimitMinimum as Any, e[SwiftUI.LabelsHiddenKey.self],
          e[SwiftUI.LabelsVisibilityKey.self], e.tableColumnHeadersHidden)
    print("env scroll", e[SwiftUI.VerticalScrollIndicatorsKey.self] == .never, e[SwiftUI.HorizontalScrollIndicatorsKey.self] == .never,
          e.scrollContentMargins, e.scrollAnchors.initialOffset as Any, e.scrollAnchors.sizeChanges as Any,
          e[SwiftUI.HardScrollEdgesKey.self].rawValue)
    print("env other", e[SwiftUI.KeyboardShortcutValueKey.self]?.key.character as Any,
          e[SwiftUI.KeyboardShortcutValueKey.self]?.modifiers.rawValue as Any, e[SwiftUI.FileDialogMessageKey.self] as Any,
          e[SwiftUI.SubmitActionsKey.self].count, e[SwiftUI.TabSidebarPartsKey.self].footer != nil,
          e[SwiftUI.TabSidebarPartsKey.self].header != nil)
}

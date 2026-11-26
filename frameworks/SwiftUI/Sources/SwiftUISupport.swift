// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import SwiftUI
import AppKit
import Accessibility

// SwiftUI API that Reef's SwiftUI lacks. Declared in module SwiftUISupport; the wrapper
// exports every symbol under SwiftUI's mangled name (see ../README.md).

// MARK: LazyState2

@propertyWrapper
public struct LazyState2<Value>: DynamicProperty {
    @State private var storage: Value?
    private let make: () -> Value

    public init(wrappedValue: @autoclosure @escaping () -> Value) {
        self.make = wrappedValue
    }

    public var wrappedValue: Value {
        if let v = storage { return v }
        return make()
    }

    public var projectedValue: Binding<Value> {
        Binding(get: { wrappedValue }, set: { storage = $0 })
    }
}

// MARK: Performance tests (SPI): the hooks do nothing

public struct PPTTestCase {
    public struct Name {
        public var rawValue: String
        public init(_ rawValue: String) { self.rawValue = rawValue }
    }
    public var parameters: [AnyHashable: Any] { [:] }
    public func startTest() {}
    public func finishTest() {}
    public func performResizeTest(onComplete: ((PPTTestCase) -> Void)? = nil) { onComplete?(self) }
    public func performScrollTest(_ proxy: ScrollViewProxy, onComplete: ((PPTTestCase) -> Void)? = nil) { onComplete?(self) }
}

extension View {
    public func onTest(_ name: PPTTestCase.Name, handler: @escaping (PPTTestCase) -> Void) -> some View { self }
}

// MARK: ScaledShape

public struct ScaledShape<Content: Shape>: Shape {
    public var shape: Content
    public var scale: CGSize
    public var anchor: UnitPoint
    public init(shape: Content, scale: CGSize, anchor: UnitPoint = .center) {
        self.shape = shape; self.scale = scale; self.anchor = anchor
    }
    public func path(in rect: CGRect) -> Path {
        let p = shape.path(in: rect)
        let ax = rect.minX + rect.width * anchor.x, ay = rect.minY + rect.height * anchor.y
        let t = CGAffineTransform(translationX: ax, y: ay).scaledBy(x: scale.width, y: scale.height).translatedBy(x: -ax, y: -ay)
        return p.applying(t)
    }
}

// MARK: Window level

public struct WindowLevel: Hashable, Sendable {
    let raw: Int
    public static var normal: WindowLevel { WindowLevel(raw: 0) }
    public static var floating: WindowLevel { WindowLevel(raw: 3) }
}

extension Scene {
    // The level is not applied yet: Reef's SwiftUI has no window level hook.
    public func windowLevel(_ level: WindowLevel) -> some Scene { self }
}

// MARK: Picker content (SPI)

public protocol PickerContent {
    associatedtype _IdentifiedView: View
    var _identifiedView: _IdentifiedView { get }
}

public struct PickerOption<Value, Content>: PickerContent {
    let value: Value
    let content: Content
    public init(value: Value, content: () -> Content) {
        self.value = value
        self.content = content()
    }
    public var _identifiedView: some View {
        let view: AnyView = (content as? any View).map { AnyView($0) } ?? AnyView(EmptyView())
        if let h = value as? any Hashable { return AnyView(tagged(view, h)) }
        return view
    }
}

private func tagged<V: Hashable>(_ view: AnyView, _ value: V) -> some View { view.tag(value) }

public struct _TuplePickerContent<A: PickerContent, B: PickerContent>: PickerContent {
    let a: A
    let b: B
    public init(_ a: A, _ b: B) { self.a = a; self.b = b }
    public var _identifiedView: some View {
        Group { a._identifiedView; b._identifiedView }
    }
}

public struct _DisabledPickerContent<A: PickerContent>: PickerContent {
    let base: A
    let isDisabled: Bool
    public init(_ base: A, _ isDisabled: Bool) { self.base = base; self.isDisabled = isDisabled }
    public var _identifiedView: some View { base._identifiedView.disabled(isDisabled) }
}

public struct _KeyboardShortcutPickerContent<A: PickerContent>: PickerContent {
    let base: A
    let key: KeyEquivalent
    let modifiers: EventModifiers
    public init(_ base: A, _ key: KeyEquivalent, _ modifiers: EventModifiers) { self.base = base; self.key = key; self.modifiers = modifiers }
    public var _identifiedView: some View { base._identifiedView.keyboardShortcut(key, modifiers: modifiers) }
}

extension PickerContent {
    public func keyboardShortcut(_ key: KeyEquivalent, modifiers: EventModifiers) -> _KeyboardShortcutPickerContent<Self> {
        _KeyboardShortcutPickerContent(self, key, modifiers)
    }
    public func disabled(_ disabled: Bool) -> _DisabledPickerContent<Self> {
        _DisabledPickerContent(self, disabled)
    }
}

public struct PickerBuilder<Selection: Hashable> {
    public struct Content<C: PickerContent>: View {
        let content: C
        public init(_ content: C) { self.content = content }
        public var body: some View { content._identifiedView }
    }
}

extension Picker {
    public init<C: PickerContent>(selection: Binding<SelectionValue>, @ViewBuilder content: () -> C, @ViewBuilder label: () -> Label)
        where Content == PickerBuilder<SelectionValue>.Content<C> {
        self.init(selection: selection, content: { PickerBuilder<SelectionValue>.Content(content()) }, label: label)
    }
}

extension Picker where Label == Text {
    public init<C: PickerContent>(_ titleKey: LocalizedStringKey, selection: Binding<SelectionValue>, @ViewBuilder content: () -> C)
        where Content == PickerBuilder<SelectionValue>.Content<C> {
        self.init(titleKey, selection: selection, content: { PickerBuilder<SelectionValue>.Content(content()) })
    }
}

// MARK: Accessibility

extension ModifiedContent where Modifier == AccessibilityAttachmentModifier {
    // Accessibility metadata is not exposed yet: the view stays as it is.
    public func accessibilityCustomContent(_ label: LocalizedStringKey, _ value: Text, importance: AXCustomContent.Importance) -> ModifiedContent<Content, AccessibilityAttachmentModifier> { self }
    public func accessibilityValue(_ valueKey: LocalizedStringKey, isEnabled: Bool) -> ModifiedContent<Content, AccessibilityAttachmentModifier> { self }
    public func accessibilityCustomAttribute(_ name: String, value: String) -> ModifiedContent<Content, AccessibilityAttachmentModifier> { self }
    public func accessibilityCustomAttribute(_ name: String, value: Bool) -> ModifiedContent<Content, AccessibilityAttachmentModifier> { self }
    public func accessibilityCustomAttribute(_ name: String, value: NSObject) -> ModifiedContent<Content, AccessibilityAttachmentModifier> { self }
}

extension AccessibilityTraits {
    public static var isKeyboardKey: AccessibilityTraits { [] }
    public static var isMathEquation: AccessibilityTraits { [] }
    public static var isToggle: AccessibilityTraits { .isButton }
}

public struct _AccessibilityIgnoresInvertColorsViewModifier: ViewModifier {
    public var active: Bool
    public init(active: Bool = true) { self.active = active }
    public func body(content: Content) -> some View { content }
}

// MARK: Window access

public struct WithCurrentWindowAction {
    public init() {}
    public func callAsFunction(_ action: (NSWindow?) -> Void) {
        action(NSApp.keyWindow ?? NSApp.mainWindow)
    }
}

extension EnvironmentValues {
    public var withCurrentWindow: WithCurrentWindowAction { WithCurrentWindowAction() }
}

// MARK: Representable reset (SPI requirement of NSViewRepresentable)

// Reef's NSViewRepresentable has no _resetNSView requirement: its requirement descriptor is answered by the
// descriptor of _overrideLayoutTraits (see build.cfg), so this default must stay a no-op: it can be reached
// through that slot with the other function's arguments.
extension NSViewRepresentable {
    public func _resetNSView(_ nsView: NSViewType, coordinator: Coordinator, destroy: () -> Void) {}
}

// MARK: Glass, scroll, list, text

public struct GlassEffectTransition: Sendable {
    public static var identity: GlassEffectTransition { GlassEffectTransition() }
}

extension View {
    public func glassEffectTransition(_ transition: GlassEffectTransition) -> some View { self }
    public func scrollEdgeEffectDisabled(_ disabled: Bool = true) -> some View { self }
    public func lineHeightMultiple(_ multiple: CGFloat) -> some View { self }
    public func listRowSeparatorTint(_ color: Color?, edges: VerticalEdge.Set = .all) -> some View { self }
}

public struct Solarium {
    public static var isEnabled: Bool {
        (Bundle.main.object(forInfoDictionaryKey: "UIDesignRequiresCompatibility") as? Bool) != true
    }
}

// MARK: Dialogs, URL and toolbar

extension View {
    public func alert<A: View, B: View>(_ title: Text, isPresented: Binding<Bool>, @ViewBuilder actions: () -> A, @ViewBuilder message: () -> B) -> some View {
        self.alert(title, isPresented: isPresented, presenting: true as Bool?, actions: { _ in actions() }, message: { _ in message() })
    }

    public func confirmationDialog<A: View>(_ title: Text, isPresented: Binding<Bool>, titleVisibility: Visibility, @ViewBuilder actions: () -> A) -> some View {
        self.confirmationDialog(title, isPresented: isPresented, titleVisibility: titleVisibility, actions: actions, message: { EmptyView() })
    }

    // The URL handler is not wired to Reef's open-URL event yet.
    public func onOpenURL(perform action: @escaping (URL) -> Void) -> some View { self }

    // View-based toolbar content is dropped for now.
    public func toolbar<A: View>(@ViewBuilder content: () -> A) -> some View { self }
}

extension Image {
    public init(_internalSystemName name: String) { self.init(systemName: name) }
}

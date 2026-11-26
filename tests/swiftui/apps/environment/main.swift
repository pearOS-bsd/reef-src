import SwiftUI

final class Store: ObservableObject { @Published var title = "store" }
struct ThemeKey: EnvironmentKey { static let defaultValue = "default-theme" }
extension EnvironmentValues { var theme: String { get { self[ThemeKey.self] } set { self[ThemeKey.self] = newValue } } }
struct Leaf: View {
    @Environment(\.theme) var theme
    @Environment(\.colorScheme) var scheme
    @Environment(\.layoutDirection) var dir
    @Environment(\.dynamicTypeSize) var dts
    @Environment(\.isEnabled) var enabled
    @Environment(\.font) var font
    @Environment(\.lineLimit) var lineLimit
    @Environment(\.displayScale) var scale
    @Environment(\.controlActiveState) var active
    @EnvironmentObject var store: Store
    var body: some View {
        Text("theme=\(theme) scheme=\(scheme) dir=\(dir) enabled=\(enabled) lineLimit=\(String(describing: lineLimit)) store=\(store.title)").frame(width: 400)
            .onAppear { Harness.note("leaf dts=\(dts) scale=\(scale) font=\(String(describing: font)) active=\(active)") }
    }
}
struct V: View {
    @StateObject var store = Store()
    var body: some View {
        VStack(spacing: 6) {
            Leaf().probe("default")
            Leaf().environment(\.theme, "dark").probe("custom")
            Leaf().environment(\.colorScheme, .dark).environment(\.layoutDirection, .rightToLeft).probe("builtin")
            Leaf().disabled(true).lineLimit(1).font(.title).probe("disabled")
            VStack { Leaf().probe("nested") }.environment(\.theme, "outer").environment(\.theme, "inner")
        }
        .environmentObject(store)
        .frame(width: 420, height: 340)
        .onAppear { DispatchQueue.main.async { store.title = "updated" } }
        .finalState("environment")
    }
}

Harness.run("environment", 420, 340) { V() }

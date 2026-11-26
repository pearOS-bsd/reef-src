import SwiftUI

struct AccentKey: EnvironmentKey { static let defaultValue = Color.gray }
struct SizeKey: EnvironmentKey { static let defaultValue: CGFloat = 10 }
extension EnvironmentValues {
    var customAccent: Color { get { self[AccentKey.self] } set { self[AccentKey.self] = newValue } }
    var customPadding: CGFloat { get { self[SizeKey.self] } set { self[SizeKey.self] = newValue } }
}
struct EnvButtonStyle: ButtonStyle {
    @Environment(\.customAccent) var accent
    @Environment(\.customPadding) var pad
    @Environment(\.controlSize) var controlSize
    @Environment(\.isEnabled) var enabled
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding(pad)
            .background(accent)
            .opacity(configuration.isPressed ? 0.5 : 1)
            .onAppear { Harness.note("style accent=\(accent) pad=\(pad) controlSize=\(controlSize) enabled=\(enabled)") }
    }
}
struct V: View {
    var body: some View {
        VStack(spacing: 12) {
            Button("Env style") { }.buttonStyle(EnvButtonStyle()).probe("env")
                .environment(\.customAccent, .red).environment(\.customPadding, 30)
            Button("Default env") { }.buttonStyle(EnvButtonStyle()).probe("default")
            Button("Builtin env") { }.buttonStyle(EnvButtonStyle()).controlSize(.large).probe("builtin")
            VStack {
                Button("Inherited") { }.buttonStyle(EnvButtonStyle()).probe("inherited")
            }.environment(\.customAccent, .green)
            Text("probe").environment(\.customPadding, 7).modifier(ReadPad()).probe("plain-read")
        }
        .frame(width: 400, height: 260)
        .finalState("button_style_env")
    }
}
struct ReadPad: ViewModifier {
    @Environment(\.customPadding) var pad
    func body(content: Content) -> some View { content.padding(pad).onAppear { Harness.note("modifier pad=\(pad)") } }
}

Harness.run("button_style_env", 400, 260) { V() }

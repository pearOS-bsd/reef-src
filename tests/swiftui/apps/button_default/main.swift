import SwiftUI

struct V: View {
    @State var count = 0
    var body: some View {
        VStack(spacing: 8) {
            Button("Plain title") { count += 1 }.probe("b1")
            Button(action: { count += 1 }) { Label("With label", systemImage: "plus") }.probe("b2")
            Button("Disabled") { count += 1 }.disabled(true).probe("b3")
            Button("Destructive", role: .destructive) { count += 1 }.probe("b4")
            Button("Cancel", role: .cancel) { count += 1 }.probe("b5")
            Button { count += 1 } label: { Text("Custom label").padding(6).background(Color.orange) }.probe("b6")
            Button("Keyboard shortcut") { count += 1 }.keyboardShortcut("k", modifiers: .command).probe("b7")
            Button("Default action") { count += 1 }.keyboardShortcut(.defaultAction).probe("b8")
            Text("count \(count)").probe("count")
        }
        .frame(width: 400, height: 300)
        .finalState("button_default")
    }
}

Harness.run("button_default", 400, 300) { V() }

import SwiftUI

struct Style: LabelStyle {
    func makeBody(configuration: Configuration) -> some View { HStack { configuration.title.bold(); configuration.icon.foregroundColor(.red) } }
}
struct V: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Link("Link to site", destination: URL(string: "https://example.invalid/")!).probe("link")
            Link(destination: URL(string: "https://example.invalid/2")!) { Label("Link label", systemImage: "link") }.probe("link2")
            Label("Custom style", systemImage: "star").labelStyle(Style()).probe("custom")
            Label("Automatic", systemImage: "bell").labelStyle(.automatic).probe("auto")
            Label("Both", systemImage: "bell").labelStyle(.titleAndIcon).probe("both")
            Text("Help tooltip").help("A tooltip").probe("help")
            Text("Selectable text").textSelection(.enabled).probe("selectable")
            Text("Accessibility").accessibilityLabel("Label").accessibilityHint("Hint").probe("a11y")
        }
        .padding()
        .frame(width: 420, height: 260, alignment: .topLeading)
        .finalState("label_link")
    }
}

Harness.run("label_link", 420, 260) { V() }

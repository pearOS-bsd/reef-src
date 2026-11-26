import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            Button("automatic") { }.buttonStyle(.automatic).probe("automatic")
            Button("bordered") { }.buttonStyle(.bordered).probe("bordered")
            Button("borderedProminent") { }.buttonStyle(.borderedProminent).probe("prominent")
            Button("borderless") { }.buttonStyle(.borderless).probe("borderless")
            Button("plain") { }.buttonStyle(.plain).probe("plain")
            Button("link") { }.buttonStyle(.link).probe("link")
            HStack {
                Button("small") { }.controlSize(.small)
                Button("regular") { }.controlSize(.regular)
                Button("large") { }.controlSize(.large)
                Button("mini") { }.controlSize(.mini)
            }.probe("sizes")
            Button("tinted") { }.buttonStyle(.borderedProminent).tint(.red).probe("tinted")
            Button("disabled prominent") { }.buttonStyle(.borderedProminent).disabled(true).probe("disabledprominent")
            Button("round") { }.buttonBorderShape(.capsule).buttonStyle(.bordered).probe("capsule")
        }
        .frame(width: 420, height: 300)
        .finalState("button_styles")
    }
}

Harness.run("button_styles", 420, 300) { V() }

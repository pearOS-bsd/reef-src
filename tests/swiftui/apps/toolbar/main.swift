import SwiftUI

struct V: View {
    @State var on = false
    var body: some View {
        VStack { Text("Content under toolbar").probe("content") }
            .frame(width: 520, height: 300)
            .toolbar {
                ToolbarItem(placement: .navigation) { Button { } label: { Image(systemName: "sidebar.left") } }
                ToolbarItem(placement: .principal) { Text("Principal") }
                ToolbarItem(placement: .primaryAction) { Button("Primary") { } }
                ToolbarItem(placement: .automatic) { Toggle("T", isOn: $on) }
                ToolbarItemGroup(placement: .secondaryAction) { Button("S1") { }; Button("S2") { } }
            }
            .navigationTitle("Toolbar title")
            .navigationSubtitle("subtitle")
            .finalState("toolbar")
    }
}

Harness.run("toolbar", 520, 300) { V() }

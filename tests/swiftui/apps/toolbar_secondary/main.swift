import SwiftUI

struct V: View {
    var body: some View {
        Text("Secondary toolbar items").frame(width: 520, height: 300)
            .toolbar { ToolbarItemGroup(placement: .secondaryAction) { Button("S1") { }; Button("S2") { } } }
            .finalState("toolbar_secondary")
    }
}

Harness.run("toolbar_secondary", 520, 300) { V() }

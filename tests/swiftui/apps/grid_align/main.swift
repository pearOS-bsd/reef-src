import SwiftUI

struct V: View {
    var body: some View {
        Grid(alignment: .topTrailing) {
            GridRow { Text("a"); Text("long text here") }
            GridRow { Text("bbbbbb").gridColumnAlignment(.leading); Text("c") }
        }.probe("grid3")
        .padding(10)
        .frame(width: 420, height: 200, alignment: .topLeading)
        .finalState("grid_align")
    }
}

Harness.run("grid_align", 420, 200) { V() }

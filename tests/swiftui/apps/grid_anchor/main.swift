import SwiftUI

struct V: View {
    var body: some View {
        Grid {
            GridRow { Text("x"); Color.gray.gridCellAnchor(.topLeading).frame(width: 40, height: 40); Text("y") }
            GridRow { Text("z").gridCellAnchor(.bottomTrailing); Text("w"); Text("v") }
        }.probe("grid4")
        .padding(10)
        .frame(width: 420, height: 200, alignment: .topLeading)
        .finalState("grid_anchor")
    }
}

Harness.run("grid_anchor", 420, 200) { V() }

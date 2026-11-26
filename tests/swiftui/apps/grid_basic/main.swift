import SwiftUI

struct V: View {
    var body: some View {
        Grid(alignment: .leading, horizontalSpacing: 8, verticalSpacing: 4) {
            GridRow { Text("Name"); Text("Value") }
            Divider().gridCellUnsizedAxes(.horizontal)
            GridRow { Text("Alpha"); Text("1") }
            GridRow { Text("Beta, longer"); Text("22") }
        }.probe("grid1")
        .padding(10)
        .frame(width: 420, height: 200, alignment: .topLeading)
        .finalState("grid_basic")
    }
}

Harness.run("grid_basic", 420, 200) { V() }

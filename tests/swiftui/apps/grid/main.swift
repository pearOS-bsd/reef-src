import SwiftUI

struct V: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Grid(alignment: .leading, horizontalSpacing: 8, verticalSpacing: 4) {
                GridRow { Text("Name"); Text("Value") }
                Divider().gridCellUnsizedAxes(.horizontal)
                GridRow { Text("Alpha"); Text("1") }
                GridRow { Text("Beta, longer"); Text("22") }
            }.probe("grid1")
            Grid {
                GridRow { Color.red.frame(width: 30, height: 30); Color.green.frame(width: 50, height: 20); Color.blue.frame(width: 20, height: 40) }
                GridRow { Color.orange.frame(width: 60, height: 10).gridCellColumns(2); Color.teal.frame(width: 10, height: 10) }
            }.probe("grid2")
            Grid(alignment: .topTrailing) {
                GridRow { Text("a"); Text("long text here") }
                GridRow { Text("bbbbbb").gridColumnAlignment(.leading); Text("c") }
            }.probe("grid3")
            Grid {
                GridRow { Text("x"); Color.gray.gridCellAnchor(.topLeading).frame(width: 40, height: 40); Text("y") }
                GridRow { Text("z").gridCellAnchor(.bottomTrailing); Text("w"); Text("v") }
            }.probe("grid4")
        }
        .padding(10)
        .frame(width: 420, height: 360, alignment: .topLeading)
        .finalState("grid")
    }
}

Harness.run("grid", 420, 360) { V() }

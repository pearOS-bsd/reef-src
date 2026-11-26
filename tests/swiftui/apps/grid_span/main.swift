import SwiftUI

struct V: View {
    var body: some View {
        Grid {
            GridRow { Color.red.frame(width: 30, height: 30); Color.green.frame(width: 50, height: 20); Color.blue.frame(width: 20, height: 40) }
            GridRow { Color.orange.frame(width: 60, height: 10).gridCellColumns(2); Color.teal.frame(width: 10, height: 10) }
        }.probe("grid2")
        .padding(10)
        .frame(width: 420, height: 200, alignment: .topLeading)
        .finalState("grid_span")
    }
}

Harness.run("grid_span", 420, 200) { V() }

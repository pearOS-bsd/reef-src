import SwiftUI

struct V: View {
    @State var vis = NavigationSplitViewVisibility.all
    @State var sel: String? = "Beta"
    let items = ["Alpha", "Beta", "Gamma"]
    var body: some View {
        NavigationSplitView(columnVisibility: $vis) {
            List(items, id: \.self, selection: $sel) { Text($0) }.navigationSplitViewColumnWidth(min: 120, ideal: 160, max: 200)
        } detail: {
            Text("Detail: \(sel ?? "none")").frame(maxWidth: .infinity, maxHeight: .infinity).probe("detail")
        }
        .probe("split")
        .frame(width: 640, height: 400)
        .onAppear { Harness.note("visibility \(vis)") }
        .finalState("nav_split")
    }
}

Harness.run("nav_split", 640, 400) { V() }

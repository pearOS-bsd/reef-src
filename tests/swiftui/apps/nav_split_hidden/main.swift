import SwiftUI

struct V: View {
    @State var vis = NavigationSplitViewVisibility.detailOnly
    var body: some View {
        NavigationSplitView(columnVisibility: $vis) {
            List(["One", "Two"], id: \.self) { Text($0) }
        } content: {
            Text("Content column").frame(maxWidth: .infinity, maxHeight: .infinity)
        } detail: {
            Text("Detail only").frame(maxWidth: .infinity, maxHeight: .infinity).probe("detail")
        }
        .probe("split")
        .frame(width: 640, height: 400)
        .onChange(of: vis) { _, v in Harness.note("visibility changed \(v)") }
        .onAppear { DispatchQueue.main.async { vis = .doubleColumn } }
        .finalState("nav_split_hidden")
    }
}

Harness.run("nav_split_hidden", 640, 400) { V() }

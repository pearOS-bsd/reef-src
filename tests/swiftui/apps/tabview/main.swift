import SwiftUI

struct V: View {
    @State var tab = 1
    var body: some View {
        TabView(selection: $tab) {
            Text("First tab").tabItem { Label("One", systemImage: "1.circle") }.tag(0)
            Text("Second tab").probe("second").tabItem { Label("Two", systemImage: "2.circle") }.tag(1)
            Text("Third tab").tabItem { Text("Three") }.tag(2)
        }
        .probe("tabs")
        .frame(width: 420, height: 300)
        .onAppear { DispatchQueue.main.async { tab = 2 } }
        .onChange(of: tab) { _, v in Harness.note("tab \(v)") }
        .finalState("tabview")
    }
}

Harness.run("tabview", 420, 300) { V() }

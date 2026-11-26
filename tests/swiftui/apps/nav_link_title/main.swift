import SwiftUI

struct V: View {
    var body: some View {
        NavigationStack {
            List { NavigationLink("Go 1", value: 1); NavigationLink("Go 2", destination: Text("Destination")) }
                .navigationDestination(for: Int.self) { Text("Detail \($0)") }
        }.frame(width: 360, height: 240).finalState("nav_link_title")
    }
}

Harness.run("nav_link_title", 360, 240) { V() }

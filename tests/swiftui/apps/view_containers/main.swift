import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            Group { Text("grouped 1"); Text("grouped 2") }.probe("group")
            AnyView(Text("anyview")).probe("anyview")
            EmptyView().probe("empty")
            Color.clear.frame(width: 10, height: 10).probe("clear")
            ForEach(0..<2, id: \.self) { Text("foreach \($0)") }
            if #available(macOS 14, *) { Text("available").probe("avail") }
        }
        .frame(width: 360, height: 200)
        .finalState("view_containers")
    }
}

Harness.run("view_containers", 360, 200) { V() }

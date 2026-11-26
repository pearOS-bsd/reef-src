import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            ViewThatFits { Text("This is a rather long single line of text").fixedSize(); Text("Short") }.frame(width: 100).probe("narrow")
            ViewThatFits { Text("This is a rather long single line of text").fixedSize(); Text("Short") }.frame(width: 400).probe("wide")
            ViewThatFits(in: .vertical) { Color.red.frame(height: 200); Color.green.frame(height: 20) }.frame(height: 50).probe("vertical")
            Group { Text("grouped 1"); Text("grouped 2") }.probe("group")
            AnyView(Text("anyview")).probe("anyview")
            EmptyView().probe("empty")
            Color.clear.frame(width: 10, height: 10).probe("clear")
            ForEach(0..<2, id: \.self) { Text("foreach \($0)") }
            if #available(macOS 14, *) { Text("available").probe("avail") }
        }
        .frame(width: 360, height: 260)
        .finalState("view_that_fits")
    }
}

Harness.run("view_that_fits", 360, 260) { V() }

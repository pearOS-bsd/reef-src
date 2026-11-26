import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 6) {
            Rectangle().fill(.red).frame(width: 100, height: 20).probe("fixed")
            Rectangle().fill(.green).frame(minWidth: 50, maxWidth: .infinity, minHeight: 10, maxHeight: 20).probe("flex")
            Rectangle().fill(.blue).frame(maxWidth: 120, maxHeight: 20).probe("maxw")
            Rectangle().fill(.orange).frame(minWidth: 200, idealWidth: 150, maxWidth: 250).frame(height: 20).probe("minmax")
            Text("fixed size text that is long").fixedSize().probe("fixedsize")
            Text("fixed horizontal only text that wraps if needed").fixedSize(horizontal: false, vertical: true).frame(width: 120).probe("fixedv")
            Color.purple.frame(width: 60, height: 20).frame(width: 140, height: 30, alignment: .trailing).probe("outer")
            Color.pink.frame(width: 300, height: 20).frame(width: 100).probe("clamp")
            Color.teal.frame(maxWidth: .infinity).frame(height: 20).padding(.horizontal, 30).probe("padflex")
            HStack(spacing: 0) {
                Color.red.frame(width: 50, height: 20).layoutPriority(1)
                Color.green.frame(minWidth: 10, maxWidth: .infinity, minHeight: 20)
                Color.blue.frame(width: 50, height: 20)
            }.frame(width: 200).probe("prio")
        }
        .frame(width: 420, height: 340)
        .finalState("frames")
    }
}

Harness.run("frames", 420, 340) { V() }

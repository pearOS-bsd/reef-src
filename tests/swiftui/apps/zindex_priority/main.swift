import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 10) {
            ZStack {
                Rectangle().fill(.red).frame(width: 80, height: 80).zIndex(0)
                Rectangle().fill(.blue).frame(width: 60, height: 60).zIndex(2)
                Rectangle().fill(.green).frame(width: 40, height: 40).zIndex(1)
            }.probe("z")
            HStack {
                Text("A long label that needs space").layoutPriority(1)
                Text("Lower priority long text here")
            }.frame(width: 260).probe("prio")
            VStack { Color.red.frame(height: 30); Color.green.frame(height: 30).ignoresSafeArea() }.frame(width: 100).probe("safe")
            Text("hidden").hidden().probe("hidden")
            Text("opacity 0").opacity(0).probe("opacity0")
            Text("clip scale").scaleEffect(2).frame(width: 60, height: 20).clipped().probe("scaleclip")
            Text("fixed").frame(width: 80, height: 20).offset(x: 20).probe("offset")
        }
        .frame(width: 360, height: 260)
        .finalState("zindex_priority")
    }
}

Harness.run("zindex_priority", 360, 260) { V() }

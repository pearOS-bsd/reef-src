import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            ViewThatFits { Text("This is a rather long single line of text").fixedSize(); Text("Short") }.frame(width: 100).probe("narrow")
            ViewThatFits { Text("This is a rather long single line of text").fixedSize(); Text("Short") }.frame(width: 400).probe("wide")
            ViewThatFits(in: .vertical) { Color.red.frame(height: 200); Color.green.frame(height: 20) }.frame(height: 50).probe("vertical")
        }
        .frame(width: 360, height: 160)
        .finalState("view_that_fits")
    }
}

Harness.run("view_that_fits", 360, 160) { V() }

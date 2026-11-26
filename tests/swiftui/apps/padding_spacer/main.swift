import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 0) {
            HStack {
                Text("L").probe("left")
                Spacer()
                Text("R").probe("right")
            }.padding().probe("row1")
            HStack {
                Text("A")
                Spacer(minLength: 100)
                Text("B")
            }.frame(width: 180).probe("minlength")
            Divider().probe("divider")
            Text("pad").padding(.top, 10).padding(.leading, 20).background(Color.yellow).probe("pad")
            Text("edges").padding([.horizontal], 12).padding(.vertical, 3).background(Color.cyan).probe("pad2")
            Text("insets").padding(EdgeInsets(top: 1, leading: 2, bottom: 3, trailing: 4)).background(Color.green).probe("insets")
            Spacer().frame(height: 10)
            VStack { Spacer(); Text("bottom") }.frame(height: 60).probe("vspacer")
        }
        .frame(width: 360, height: 300)
        .finalState("padding_spacer")
    }
}

Harness.run("padding_spacer", 360, 300) { V() }

import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 14) {
            Text("overlay").padding().background(Color.yellow).overlay(Rectangle().stroke(.red, lineWidth: 2)).probe("o1")
            Text("badge").padding(8).overlay(alignment: .topTrailing) { Circle().fill(.red).frame(width: 12, height: 12) }.probe("o2")
            Color.blue.frame(width: 80, height: 40).overlay { Text("on blue").foregroundColor(.white) }.probe("o3")
            Text("background shape").padding().background(RoundedRectangle(cornerRadius: 8).fill(.green)).probe("b1")
            Text("background align").frame(width: 120, height: 40).background(alignment: .bottomLeading) { Rectangle().fill(.orange).frame(width: 20, height: 20) }.probe("b2")
            Text("bordered").padding(4).border(.purple, width: 2).probe("b3")
            Text("style bg").padding().background(.quaternary, in: RoundedRectangle(cornerRadius: 6)).probe("b4")
            Text("big bg ignores").background(Color.pink.frame(width: 200, height: 10)).probe("b5")
            Text("clipped").frame(width: 40, height: 14).background(Color.cyan).clipped().probe("c1")
            Text("contentShape").padding().contentShape(Rectangle()).onTapGesture { }.probe("c2")
        }
        .frame(width: 400, height: 320)
        .finalState("overlay_background")
    }
}

Harness.run("overlay_background", 400, 320) { V() }

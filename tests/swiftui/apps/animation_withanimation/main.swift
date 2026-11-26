import SwiftUI

struct V: View {
    @State var on = false
    var body: some View {
        VStack(spacing: 8) {
            Rectangle().fill(.red).frame(width: on ? 200 : 50, height: 30).animation(.easeInOut(duration: 0.1), value: on).probe("w")
            Circle().fill(.blue).frame(width: 30, height: 30).offset(x: on ? 80 : 0).animation(.spring(duration: 0.1), value: on).probe("c")
            Text("on=\(on.description)").probe("t")
        }
        .frame(width: 360, height: 260)
        .onAppear { withAnimation(.linear(duration: 0.1)) { on = true } }
        .onChange(of: on) { _, v in Harness.note("on \(v)") }
        .finalState("animation_withanimation")
    }
}

Harness.run("animation_withanimation", 360, 260) { V() }

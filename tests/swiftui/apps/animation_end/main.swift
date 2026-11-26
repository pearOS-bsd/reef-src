import SwiftUI

struct V: View {
    @State var on = false
    @State var angle = 0.0
    @State var width: CGFloat = 40
    var body: some View {
        VStack(spacing: 8) {
            Rectangle().fill(.red).frame(width: width, height: 30).probe("grow")
            Rectangle().fill(.blue).frame(width: 40, height: 30).rotationEffect(.degrees(angle)).probe("rot")
            Circle().fill(.green).frame(width: 30, height: 30).opacity(on ? 1 : 0.2).offset(x: on ? 40 : 0).probe("circle")
            if on { Text("appeared").transition(.opacity).probe("appeared") }
            Text("disabled animation").animation(nil, value: on).probe("none")
        }
        .frame(width: 360, height: 260)
        .onAppear {
            var t = Transaction(); t.disablesAnimations = true
            withTransaction(t) { on = true; angle = 90; width = 120 }
        }
        .transaction { $0.animation = nil }
        .finalState("animation_end")
    }
}

Harness.run("animation_end", 360, 260) { V() }

import SwiftUI

struct V: View {
    @State var show = false
    @State var second = false
    var body: some View {
        VStack(spacing: 8) {
            if show { Text("opacity").transition(.opacity).probe("t1") }
            if show { Text("move").transition(.move(edge: .leading)).probe("t2") }
            if show { Text("scale").transition(.scale).probe("t3") }
            if show { Text("asymmetric").transition(.asymmetric(insertion: .slide, removal: .opacity)).probe("t4") }
            if second { Text("second").probe("t5") } else { Text("replaced").probe("t6") }
            Text("show=\(show.description)").probe("state")
        }
        .frame(width: 360, height: 280)
        .onAppear { DispatchQueue.main.async { show = true; DispatchQueue.main.async { second = true } } }
        .finalState("transitions")
    }
}

Harness.run("transitions", 360, 280) { V() }

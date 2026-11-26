import SwiftUI

struct V: View {
    enum F { case a, b }
    @FocusState var focus: F?
    @State var a = "a"
    @State var b = "b"
    var body: some View {
        VStack(spacing: 8) {
            TextField("a", text: $a).focused($focus, equals: .a).probe("a")
            TextField("b", text: $b).focused($focus, equals: .b).probe("b")
            Text("focus=\(String(describing: focus))").probe("state")
        }
        .padding()
        .frame(width: 360, height: 220)
        .onAppear { DispatchQueue.main.async { focus = .b } }
        .onChange(of: focus) { _, v in Harness.note("focus \(String(describing: v))") }
        .finalState("focus_state")
    }
}

Harness.run("focus_state", 360, 220) { V() }

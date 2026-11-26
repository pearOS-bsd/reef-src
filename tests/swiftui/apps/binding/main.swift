import SwiftUI

struct Child: View {
    @Binding var value: Int
    @Binding var text: String
    var body: some View {
        VStack { Text("child \(value) \(text)").probe("child"); Stepper("v", value: $value) }
            .onAppear { DispatchQueue.main.async { value += 10; text = "set by child" } }
    }
}
struct V: View {
    @State var n = 1
    @State var s = "parent"
    var body: some View {
        VStack(spacing: 8) {
            Text("parent \(n) \(s)").probe("parent")
            Child(value: $n, text: $s)
            Child(value: .constant(7), text: Binding(get: { s.uppercased() }, set: { s = $0 }))
        }
        .frame(width: 360, height: 260)
        .onChange(of: n) { _, v in Harness.note("n=\(v)") }
        .finalState("binding")
    }
}

Harness.run("binding", 360, 260) { V() }

import SwiftUI

struct V: View {
    @State var a = true
    @State var b = false
    @State var c = true
    @State var d = false
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Toggle("Switch style", isOn: $a).toggleStyle(.switch).probe("switch")
            Toggle("Checkbox style", isOn: $b).toggleStyle(.checkbox).probe("checkbox")
            Toggle("Button style", isOn: $c).toggleStyle(.button).probe("button")
            Toggle("Disabled", isOn: $d).disabled(true).probe("disabled")
            Toggle(isOn: $a) { Label("Label content", systemImage: "wifi") }.probe("label")
            Toggle("", isOn: $b).labelsHidden().probe("hidden")
            Text("a=\(a) b=\(b) c=\(c) d=\(d)").probe("state")
        }
        .padding()
        .frame(width: 420, height: 300, alignment: .topLeading)
        .onAppear { DispatchQueue.main.async { b = true; DispatchQueue.main.async { a = false } } }
        .onChange(of: a) { _, v in Harness.note("a changed \(v)") }
        .finalState("toggle")
    }
}

Harness.run("toggle", 420, 300) { V() }

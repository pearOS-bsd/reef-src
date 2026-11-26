import SwiftUI

struct V: View {
    @State var a = "initial"
    @State var b = ""
    @State var secret = "hunter2"
    @State var multi = "line one\nline two"
    @State var num = 42
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            TextField("Placeholder", text: $a).probe("tf")
            TextField("Empty with prompt", text: $b, prompt: Text("type here")).probe("prompt")
            TextField("Rounded", text: $a).textFieldStyle(.roundedBorder).probe("rounded")
            TextField("Plain", text: $a).textFieldStyle(.plain).probe("plain")
            TextField("Fixed", text: $a).frame(width: 100).probe("fixed")
            SecureField("Password", text: $secret).probe("secure")
            TextField("Number", value: $num, format: .number).probe("number")
            TextField("Vertical", text: $multi, axis: .vertical).lineLimit(1...3).probe("vertical")
            TextEditor(text: $multi).frame(height: 50).probe("editor")
            TextField("Disabled", text: $a).disabled(true).probe("disabled")
            Text("a=\(a) num=\(num)").probe("state")
        }
        .padding()
        .frame(width: 420, height: 340, alignment: .topLeading)
        .onAppear { DispatchQueue.main.async { a = "changed" } }
        .finalState("textfield")
    }
}

Harness.run("textfield", 420, 340) { V() }

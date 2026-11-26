import SwiftUI

struct V: View {
    @State var s = 1
    @State var t = "b"
    var body: some View {
        VStack(spacing: 12) {
            Picker("Seg", selection: $s) { Text("One").tag(0); Text("Two").tag(1); Text("Three").tag(2) }.pickerStyle(.segmented).probe("seg")
            Picker("Seg2", selection: $t) { Text("a").tag("a"); Text("b").tag("b") }.pickerStyle(.segmented).frame(width: 150).probe("seg2")
            Picker("Icons", selection: $s) { Image(systemName: "list.bullet").tag(0); Image(systemName: "square.grid.2x2").tag(1) }.pickerStyle(.segmented).labelsHidden().frame(width: 100).probe("icons")
            Picker("Radio", selection: $s) { Text("R0").tag(0); Text("R1").tag(1) }.pickerStyle(.radioGroup).probe("radio")
            Picker("Inline", selection: $s) { Text("I0").tag(0); Text("I1").tag(1) }.pickerStyle(.inline).probe("inline")
            Text("s=\(s) t=\(t)").probe("state")
        }
        .padding()
        .frame(width: 420, height: 300)
        .onAppear { DispatchQueue.main.async { s = 2 } }
        .finalState("picker_segmented")
    }
}

Harness.run("picker_segmented", 420, 300) { V() }

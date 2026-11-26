import SwiftUI

enum Fruit: String, CaseIterable, Identifiable { case apple, banana, cherry; var id: String { rawValue } }
struct V: View {
    @State var f: Fruit = .banana
    @State var n = 2
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Picker("Fruit", selection: $f) { ForEach(Fruit.allCases) { Text($0.rawValue.capitalized).tag($0) } }.pickerStyle(.menu).probe("menu")
            Picker("Number", selection: $n) { Text("One").tag(1); Text("Two").tag(2); Text("Three").tag(3) }.probe("number")
            Picker("", selection: $f) { ForEach(Fruit.allCases) { Text($0.rawValue).tag($0) } }.labelsHidden().probe("nolabel")
            Picker("Disabled", selection: $n) { Text("One").tag(1); Text("Two").tag(2) }.disabled(true).probe("disabled")
            Picker("Popup items", selection: $f) {
                Section("Group A") { Text("apple").tag(Fruit.apple) }
                Section("Group B") { Text("banana").tag(Fruit.banana); Text("cherry").tag(Fruit.cherry) }
            }.probe("sections")
            Text("f=\(f.rawValue) n=\(n)").probe("state")
        }
        .padding()
        .frame(width: 420, height: 300, alignment: .topLeading)
        .onAppear { DispatchQueue.main.async { f = .cherry } }
        .finalState("picker_menu")
    }
}

Harness.run("picker_menu", 420, 300) { V() }

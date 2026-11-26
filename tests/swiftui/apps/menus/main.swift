import SwiftUI

struct V: View {
    @State var sel = 1
    var body: some View {
        VStack(spacing: 10) {
            Menu("Menu title") { Button("First") { }; Button("Second") { }; Divider(); Button("Delete", role: .destructive) { } }.probe("menu")
            Menu { Button("A") { }; Menu("Submenu") { Button("Nested") { } } } label: { Label("Label menu", systemImage: "ellipsis.circle") }.probe("menu2")
            Menu("Primary action") { Button("x") { } } primaryAction: { }.probe("menu3")
            Menu("Borderless") { Button("y") { } }.menuStyle(.borderlessButton).probe("menu4")
            Menu("No indicator") { Button("z") { } }.menuIndicator(.hidden).probe("menu5")
            Menu("Picker inside") { Picker("p", selection: $sel) { Text("1").tag(1); Text("2").tag(2) }; Toggle("t", isOn: .constant(true)) }.probe("menu6")
            Text("context menu host").padding().background(Color.gray.opacity(0.2)).contextMenu { Button("Ctx 1") { }; Button("Ctx 2") { } }.probe("ctx")
        }
        .frame(width: 420, height: 300)
        .finalState("menus")
    }
}

Harness.run("menus", 420, 300) { V() }

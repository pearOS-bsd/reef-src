import SwiftUI

struct Item: Identifiable, Hashable { let id: Int; let name: String }
struct V: View {
    let items = (1...6).map { Item(id: $0, name: "Item \($0)") }
    @State var sel: Int? = 3
    var body: some View {
        HStack(spacing: 0) {
            List(items, selection: $sel) { item in Text(item.name).probe("row\(item.id)") }
                .frame(width: 140).probe("list1")
            List {
                Section("First") { ForEach(items.prefix(2)) { Text($0.name) } }
                Section("Second") { ForEach(items.suffix(2)) { Text($0.name) } }
                Text("Static row")
                ForEach(0..<2, id: \.self) { i in Text("Range \(i)") }
            }.listStyle(.sidebar).frame(width: 140).probe("list2")
            List { ForEach(items.prefix(3)) { Text($0.name) } }.listStyle(.inset(alternatesRowBackgrounds: true)).frame(width: 140).probe("list3")
        }
        .frame(width: 420, height: 400)
        .onAppear { DispatchQueue.main.async { sel = 5 } }
        .onChange(of: sel) { _, v in Harness.note("selection \(String(describing: v))") }
        .finalState("list_foreach")
    }
}

Harness.run("list_foreach", 420, 400) { V() }

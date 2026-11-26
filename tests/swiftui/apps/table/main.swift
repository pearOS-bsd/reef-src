import SwiftUI

struct Person: Identifiable { let id: Int; let name: String; let age: Int }
struct V: View {
    let people = [Person(id: 1, name: "Ann", age: 31), Person(id: 2, name: "Bob", age: 25), Person(id: 3, name: "Cy", age: 47)]
    @State var sel: Set<Int> = [2]
    var body: some View {
        Table(people, selection: $sel) {
            TableColumn("Name", value: \.name)
            TableColumn("Age") { Text("\($0.age)") }.width(60)
            TableColumn("Id") { Text("#\($0.id)") }
        }
        .probe("table")
        .frame(width: 520, height: 300)
        .finalState("table")
    }
}

Harness.run("table", 520, 300) { V() }

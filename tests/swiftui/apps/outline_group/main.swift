import SwiftUI

struct Node: Identifiable { let id: String; var children: [Node]? = nil }
struct V: View {
    let tree = [Node(id: "Root A", children: [Node(id: "A1"), Node(id: "A2", children: [Node(id: "A2a")])]), Node(id: "Root B", children: [Node(id: "B1")]), Node(id: "Leaf")]
    var body: some View {
        List { OutlineGroup(tree, children: \.children) { Text($0.id) } }.frame(height: 160).probe("outline")
            .frame(width: 420, height: 200)
            .finalState("outline_group")
    }
}

Harness.run("outline_group", 420, 200) { V() }

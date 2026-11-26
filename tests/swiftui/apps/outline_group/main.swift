import SwiftUI

struct Node: Identifiable { let id: String; var children: [Node]? = nil }
struct V: View {
    let tree = [Node(id: "Root A", children: [Node(id: "A1"), Node(id: "A2", children: [Node(id: "A2a")])]), Node(id: "Root B", children: [Node(id: "B1")]), Node(id: "Leaf")]
    var body: some View {
        VStack(spacing: 8) {
            List(tree, children: \.children) { Text($0.id) }.frame(height: 140).probe("list")
            List { OutlineGroup(tree, children: \.children) { Text($0.id) } }.frame(height: 100).probe("outline")
        }
        .frame(width: 420, height: 320)
        .finalState("outline_group")
    }
}

Harness.run("outline_group", 420, 320) { V() }

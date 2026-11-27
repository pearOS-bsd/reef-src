import SwiftUI

struct V: View {
    @State var path: [Int] = []
    var body: some View {
        NavigationStack(path: $path) {
            List(1...3, id: \.self) { i in NavigationLink(value: i) { Text("Go \(i)") } }
                .navigationTitle("Root")
                .navigationDestination(for: Int.self) { i in
                    VStack { Text("Detail \(i)").font(.title).probe("detail"); NavigationLink(value: i * 10) { Text("Deeper") } }.navigationTitle("Detail \(i)")
                }
        }
        .probe("stack")
        .frame(width: 420, height: 340)
        .onAppear { DispatchQueue.main.async { path.append(2); DispatchQueue.main.async { path.append(20) } } }
        .onChange(of: path.count) { _, c in Harness.note("path count \(c)") }
        .finalState("nav_stack")
    }
}

Harness.run("nav_stack", 420, 340) { V() }

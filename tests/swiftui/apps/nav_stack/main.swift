import SwiftUI

struct V: View {
    @State var path = NavigationPath()
    var body: some View {
        NavigationStack(path: $path) {
            List(1...3, id: \.self) { i in NavigationLink("Go \(i)", value: i) }
                .navigationTitle("Root")
                .navigationDestination(for: Int.self) { i in
                    VStack { Text("Detail \(i)").font(.title).probe("detail"); NavigationLink("Deeper", value: i * 10) }.navigationTitle("Detail \(i)")
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

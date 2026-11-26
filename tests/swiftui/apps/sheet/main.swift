import SwiftUI

struct V: View {
    @State var show = false
    @State var item: Int? = nil
    var body: some View {
        Text("Sheet host").frame(width: 420, height: 300)
            .sheet(isPresented: $show) {
                VStack { Text("Sheet content").probe("sheet"); Button("Close") { show = false } }.frame(width: 240, height: 120).onAppear { Harness.note("sheet appeared") }
            }
            .sheet(item: $item) { i in Text("Item sheet \(i)").padding() }
            .onAppear { DispatchQueue.main.async { show = true } }
            .onChange(of: show) { _, v in Harness.note("show \(v)") }
            .finalState("sheet")
    }
}
extension Int: @retroactive Identifiable { public var id: Int { self } }

Harness.run("sheet", 420, 300) { V() }

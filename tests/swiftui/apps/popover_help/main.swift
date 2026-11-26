import SwiftUI

struct V: View {
    @State var show = false
    var body: some View {
        Button("Anchor") { }.frame(width: 420, height: 300)
            .popover(isPresented: $show, arrowEdge: .bottom) { Text("Popover content").padding().onAppear { Harness.note("popover appeared") } }
            .onAppear { DispatchQueue.main.async { show = true } }
            .onChange(of: show) { _, v in Harness.note("show \(v)") }
            .finalState("popover_help")
    }
}

Harness.run("popover_help", 420, 300) { V() }

import SwiftUI

struct V: View {
    @State var show = false
    var body: some View {
        Text("Dialog host").frame(width: 420, height: 300)
            .confirmationDialog("Choose an action", isPresented: $show, titleVisibility: .visible) {
                Button("Delete", role: .destructive) { }
                Button("Rename") { }
                Button("Cancel", role: .cancel) { }
            } message: { Text("Pick one") }
            .onAppear { DispatchQueue.main.async { show = true } }
            .onChange(of: show) { _, v in Harness.note("show \(v)") }
            .finalState("confirmation_dialog")
    }
}

Harness.run("confirmation_dialog", 420, 300) { V() }

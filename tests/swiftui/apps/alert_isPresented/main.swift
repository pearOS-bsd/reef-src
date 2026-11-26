import SwiftUI

struct V: View {
    @State var show = false
    var body: some View {
        Text("Alert test").frame(width: 400, height: 260)
            .alert("Alert title", isPresented: $show) {
                Button("OK") { }
                Button("Cancel", role: .cancel) { }
            } message: { Text("The message") }
            .onAppear { DispatchQueue.main.async { show = true } }
            .onChange(of: show) { _, v in Harness.note("show \(v)") }
            .finalState("alert_isPresented")
    }
}

Harness.run("alert_isPresented", 400, 260) { V() }

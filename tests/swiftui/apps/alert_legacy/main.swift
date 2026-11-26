import SwiftUI

struct V: View {
    @State var show = false
    var body: some View {
        Text("Legacy alert").frame(width: 400, height: 260)
            .alert(isPresented: $show) {
                Alert(title: Text("Legacy"), message: Text("msg"), primaryButton: .default(Text("Yes")), secondaryButton: .cancel())
            }
            .onAppear { DispatchQueue.main.async { show = true } }
            .finalState("alert_legacy")
    }
}

Harness.run("alert_legacy", 400, 260) { V() }

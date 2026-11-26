import SwiftUI

struct V: View {
    @State var size = CGSize.zero
    @State var changes = 0
    @State var w: CGFloat = 100
    var body: some View {
        VStack(spacing: 8) {
            Color.blue.frame(width: w, height: 30)
                .onGeometryChange(for: CGSize.self) { $0.size } action: { s in size = s; changes += 1; Harness.note("geometry action size=\(s)") }
                .probe("watched")
            Text("size=\(Int(size.width))x\(Int(size.height)) changes>0=\(changes > 0)").probe("result")
            Color.green.frame(width: 50, height: 10)
                .onGeometryChange(for: Bool.self) { $0.size.width > 40 } action: { Harness.note("wide=\($0)") }
        }
        .frame(width: 360, height: 240)
        .onAppear { DispatchQueue.main.async { w = 150 } }
        .finalState("geometry_action")
    }
}

Harness.run("geometry_action", 360, 240) { V() }

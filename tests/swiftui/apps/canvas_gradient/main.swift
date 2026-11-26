import SwiftUI

struct V: View {
    var body: some View {
        Canvas { ctx, size in
            ctx.fill(Path(CGRect(x: 10, y: 10, width: 60, height: 40)), with: .linearGradient(Gradient(colors: [.green, .yellow]), startPoint: CGPoint(x: 10, y: 10), endPoint: CGPoint(x: 70, y: 10)))
        }.frame(width: 300, height: 90).probe("canvas")
        .frame(width: 360, height: 160)
        .finalState("canvas_gradient")
    }
}

Harness.run("canvas_gradient", 360, 160) { V() }

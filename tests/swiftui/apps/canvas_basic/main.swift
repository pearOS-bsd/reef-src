import SwiftUI

struct V: View {
    var body: some View {
        Canvas { ctx, size in
            ctx.fill(Path(CGRect(x: 10, y: 10, width: 60, height: 40)), with: .color(.red))
            ctx.stroke(Path(ellipseIn: CGRect(x: 80, y: 10, width: 60, height: 40)), with: .color(.blue), lineWidth: 3)
            ctx.draw(Text("canvas text"), at: CGPoint(x: size.width / 2, y: 70))
            var c2 = ctx; c2.translateBy(x: 230, y: 10); c2.rotate(by: .degrees(20)); c2.fill(Path(CGRect(x: 0, y: 0, width: 40, height: 20)), with: .color(.purple))
        }.frame(width: 300, height: 90).background(Color.gray.opacity(0.2)).probe("canvas")
        .frame(width: 360, height: 160)
        .finalState("canvas_basic")
    }
}

Harness.run("canvas_basic", 360, 160) { V() }

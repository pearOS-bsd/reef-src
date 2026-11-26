import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            Canvas { ctx, size in
                ctx.fill(Path(CGRect(x: 10, y: 10, width: 60, height: 40)), with: .color(.red))
                ctx.stroke(Path(ellipseIn: CGRect(x: 80, y: 10, width: 60, height: 40)), with: .color(.blue), lineWidth: 3)
                ctx.draw(Text("canvas text"), at: CGPoint(x: size.width / 2, y: 70))
                ctx.fill(Path(CGRect(x: 160, y: 10, width: 60, height: 40)), with: .linearGradient(Gradient(colors: [.green, .yellow]), startPoint: CGPoint(x: 160, y: 10), endPoint: CGPoint(x: 220, y: 10)))
                var c2 = ctx; c2.translateBy(x: 230, y: 10); c2.rotate(by: .degrees(20)); c2.fill(Path(CGRect(x: 0, y: 0, width: 40, height: 20)), with: .color(.purple))
            }.frame(width: 300, height: 90).background(Color.gray.opacity(0.2)).probe("canvas")
            Canvas { ctx, size in
                ctx.clip(to: Path(ellipseIn: CGRect(origin: .zero, size: size)))
                ctx.fill(Path(CGRect(origin: .zero, size: size)), with: .color(.orange))
                if let s = ctx.resolveSymbol(id: 1) { ctx.draw(s, at: CGPoint(x: 40, y: 20)) }
            } symbols: { Text("symbol").tag(1) }.frame(width: 80, height: 40).probe("clipped")
        }
        .frame(width: 360, height: 300)
        .finalState("canvas")
    }
}

Harness.run("canvas", 360, 300) { V() }

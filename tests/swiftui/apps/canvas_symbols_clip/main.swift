import SwiftUI

struct V: View {
    var body: some View {
        Canvas { ctx, size in
            ctx.clip(to: Path(ellipseIn: CGRect(origin: .zero, size: size)))
            ctx.fill(Path(CGRect(origin: .zero, size: size)), with: .color(.orange))
            if let s = ctx.resolveSymbol(id: 1) { ctx.draw(s, at: CGPoint(x: 40, y: 20)) }
        } symbols: { Text("symbol").tag(1) }.frame(width: 80, height: 40).probe("clipped")
        .frame(width: 360, height: 160)
        .finalState("canvas_symbols_clip")
    }
}

Harness.run("canvas_symbols_clip", 360, 160) { V() }

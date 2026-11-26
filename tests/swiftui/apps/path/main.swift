import SwiftUI

struct Star: Shape {
    func path(in rect: CGRect) -> Path {
        var p = Path()
        let c = CGPoint(x: rect.midX, y: rect.midY), r = min(rect.width, rect.height) / 2
        for i in 0..<5 {
            let a = Double(i) * 4 * .pi / 5 - .pi / 2
            let pt = CGPoint(x: c.x + r * CGFloat(cos(a)), y: c.y + r * CGFloat(sin(a)))
            if i == 0 { p.move(to: pt) } else { p.addLine(to: pt) }
        }
        p.closeSubpath()
        return p
    }
}
struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            Star().fill(.yellow).frame(width: 80, height: 80).probe("star")
            Star().stroke(.red, lineWidth: 2).frame(width: 80, height: 80).probe("starstroke")
            Path { p in
                p.move(to: CGPoint(x: 0, y: 40))
                p.addQuadCurve(to: CGPoint(x: 80, y: 40), control: CGPoint(x: 40, y: 0))
                p.addCurve(to: CGPoint(x: 160, y: 40), control1: CGPoint(x: 100, y: 80), control2: CGPoint(x: 140, y: 0))
            }.stroke(.blue, style: StrokeStyle(lineWidth: 3, lineCap: .round, lineJoin: .round)).frame(width: 160, height: 80).probe("curves")
            Path(ellipseIn: CGRect(x: 0, y: 0, width: 60, height: 30)).fill(.green).frame(width: 60, height: 30).probe("ellipse")
            Text("bounds").probe("bounds-owner")
            Star().path(in: CGRect(x: 0, y: 0, width: 100, height: 100)).boundingRect.size.width.description.asText.probe("boundingRect")
        }
        .frame(width: 300, height: 300)
        .finalState("path")
    }
}
extension String { var asText: Text { Text(self) } }

Harness.run("path", 300, 300) { V() }

import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            HStack(spacing: 8) {
                Circle().fill(.red).frame(width: 50, height: 50).probe("circle")
                Capsule().fill(.green).frame(width: 80, height: 40).probe("capsule")
                RoundedRectangle(cornerRadius: 12).fill(.blue).frame(width: 60, height: 50).probe("rrect")
                Ellipse().fill(.orange).frame(width: 70, height: 40).probe("ellipse")
            }
            HStack(spacing: 8) {
                Circle().stroke(.red, lineWidth: 4).frame(width: 50, height: 50).probe("stroke")
                Rectangle().strokeBorder(.blue, lineWidth: 6).frame(width: 50, height: 50).probe("border")
                RoundedRectangle(cornerRadius: 10, style: .continuous).stroke(.green, style: StrokeStyle(lineWidth: 3, dash: [5, 3])).frame(width: 60, height: 50).probe("dash")
                Capsule().strokeBorder(.purple, lineWidth: 3).frame(width: 70, height: 30).probe("capsuleborder")
            }
            HStack(spacing: 8) {
                Rectangle().fill(.gray).frame(width: 40, height: 40).clipShape(Circle()).probe("clipcircle")
                Rectangle().fill(.pink).frame(width: 40, height: 40).cornerRadius(10).probe("corner")
                Circle().trim(from: 0, to: 0.25).stroke(.red, lineWidth: 5).frame(width: 50, height: 50).probe("trim")
                UnevenRoundedRectangle(topLeadingRadius: 15, bottomLeadingRadius: 0, bottomTrailingRadius: 15, topTrailingRadius: 0).fill(.teal).frame(width: 50, height: 40).probe("uneven")
                Rectangle().fill(.yellow).frame(width: 50, height: 50).rotationEffect(.degrees(45)).probe("rotated")
            }
            HStack(spacing: 8) {
                Circle().fill(.red).overlay(Circle().stroke(.black, lineWidth: 2)).frame(width: 40, height: 40).probe("overlaystroke")
                Circle().fill(.blue, strokeBorder: .white, lineWidth: 3).frame(width: 40, height: 40).probe("fillborder")
                Rectangle().fill(.green).scaleEffect(0.5).frame(width: 40, height: 40).probe("scaled")
                Rectangle().fill(.red).offset(x: 10, y: 5).frame(width: 30, height: 30).probe("offset")
            }
        }
        .frame(width: 400, height: 360)
        .finalState("shapes")
    }
}
extension InsettableShape {
    func fill<S: ShapeStyle, B: ShapeStyle>(_ fill: S, strokeBorder: B, lineWidth: CGFloat) -> some View {
        self.fill(fill).overlay(self.strokeBorder(strokeBorder, lineWidth: lineWidth))
    }
}

Harness.run("shapes", 400, 360) { V() }

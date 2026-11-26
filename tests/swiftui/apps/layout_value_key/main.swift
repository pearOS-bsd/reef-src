import SwiftUI

struct Radial: Layout {
    var radius: CGFloat = 60
    func sizeThatFits(proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) -> CGSize { CGSize(width: radius * 2 + 40, height: radius * 2 + 40) }
    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) {
        let c = CGPoint(x: bounds.midX, y: bounds.midY)
        for (i, s) in subviews.enumerated() {
            let a = Double(i) / Double(max(subviews.count, 1)) * 2 * .pi
            s.place(at: CGPoint(x: c.x + radius * CGFloat(cos(a)), y: c.y + radius * CGFloat(sin(a))), anchor: .center, proposal: .unspecified)
        }
    }
}
struct Flow: Layout {
    struct Cache { var sizes: [CGSize] = [] }
    func makeCache(subviews: Subviews) -> Cache { Cache(sizes: subviews.map { $0.sizeThatFits(.unspecified) }) }
    func sizeThatFits(proposal: ProposedViewSize, subviews: Subviews, cache: inout Cache) -> CGSize {
        let w = proposal.width ?? 200
        var x: CGFloat = 0, y: CGFloat = 0, rowH: CGFloat = 0
        for s in cache.sizes { if x + s.width > w { x = 0; y += rowH + 4; rowH = 0 }; x += s.width + 4; rowH = max(rowH, s.height) }
        return CGSize(width: w, height: y + rowH)
    }
    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize, subviews: Subviews, cache: inout Cache) {
        var x = bounds.minX, y = bounds.minY, rowH: CGFloat = 0
        for (i, s) in subviews.enumerated() {
            let sz = cache.sizes[i]
            if x + sz.width > bounds.maxX { x = bounds.minX; y += rowH + 4; rowH = 0 }
            s.place(at: CGPoint(x: x, y: y), proposal: ProposedViewSize(sz)); x += sz.width + 4; rowH = max(rowH, sz.height)
        }
    }
}
struct WeightKey: LayoutValueKey { static let defaultValue: CGFloat = 1 }
struct Weighted: Layout {
    func sizeThatFits(proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) -> CGSize { CGSize(width: proposal.width ?? 100, height: 20) }
    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) {
        let total = subviews.reduce(0) { $0 + $1[WeightKey.self] }
        var x = bounds.minX
        for s in subviews { let w = bounds.width * s[WeightKey.self] / total; s.place(at: CGPoint(x: x, y: bounds.minY), proposal: ProposedViewSize(width: w, height: bounds.height)); x += w }
    }
}

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            Weighted { Color.red.layoutValue(key: WeightKey.self, value: 1); Color.green.layoutValue(key: WeightKey.self, value: 3) }.frame(width: 200).probe("weighted")
        }
        .frame(width: 420, height: 260)
        .finalState("layout_value_key")
    }
}

Harness.run("layout_value_key", 420, 260) { V() }

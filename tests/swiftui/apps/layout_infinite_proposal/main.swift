import SwiftUI

// A Layout that asks its children for their size under several proposals and prints the answers.
// Apple: a child with frame(maxWidth: 48) answers 48 to an infinite width proposal, 20 to 20, and 4 / 0 as its minimum.
struct Asker: Layout {
    func sizeThatFits(proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) -> CGSize { CGSize(width: 100, height: 60) }
    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) {
        let props: [(String, ProposedViewSize)] = [
            ("zero", .zero), ("unspecified", .unspecified), ("infinity", .infinity),
            ("w20", ProposedViewSize(width: 20, height: 48)), ("w100", ProposedViewSize(width: 100, height: 48)),
            ("winf", ProposedViewSize(width: .infinity, height: 48)),
        ]
        for (i, s) in subviews.enumerated() {
            for (n, p) in props { Harness.note("child\(i) \(n) -> \(s.sizeThatFits(p))") }
            Harness.note("child\(i) spacing/priority \(s.priority)")
            s.place(at: bounds.origin, proposal: ProposedViewSize(width: 48, height: 48))
        }
    }
}

struct V: View {
    var body: some View {
        VStack {
            Asker {
                Color.red.frame(maxWidth: 48, maxHeight: 48)
                Color.green.frame(minWidth: 4, maxWidth: 48, minHeight: 4, maxHeight: 48)
                Text("Label").frame(minWidth: 10, maxWidth: 48)
                Color.blue.aspectRatio(1, contentMode: .fit)
                Color.orange
                Text("plain")
            }.probe("asker")
        }
        .frame(width: 300, height: 200)
        .finalState("layout_infinite_proposal")
    }
}

Harness.run("layout_infinite_proposal", 300, 200) { V() }

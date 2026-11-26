import SwiftUI

struct BoundsKey: PreferenceKey { static var defaultValue: Anchor<CGRect>? = nil; static func reduce(value: inout Anchor<CGRect>?, nextValue: () -> Anchor<CGRect>?) { value = value ?? nextValue() } }
struct V: View {
    var body: some View {
        Text("anchor").padding(10).anchorPreference(key: BoundsKey.self, value: .bounds) { $0 }.overlayPreferenceValue(BoundsKey.self) { a in
            GeometryReader { g in if let a { Color.red.opacity(0.3).frame(width: g[a].width, height: g[a].height).offset(x: g[a].minX, y: g[a].minY).onAppear { Harness.note("anchor rect \(g[a])") } } }
        }.probe("anchor")
        .frame(width: 360, height: 200)
        .finalState("anchor_preference")
    }
}

Harness.run("anchor_preference", 360, 200) { V() }

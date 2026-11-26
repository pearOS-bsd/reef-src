import SwiftUI

struct WidthKey: PreferenceKey {
    static var defaultValue: [CGFloat] = []
    static func reduce(value: inout [CGFloat], nextValue: () -> [CGFloat]) { value += nextValue() }
}
struct V: View {
    @State var widths: [CGFloat] = []
    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            ForEach(["short", "a longer string", "mid size"], id: \.self) { s in
                Text(s).background(GeometryReader { g in Color.clear.preference(key: WidthKey.self, value: [g.size.width]) })
            }
            Text("widths \(widths.map { Int($0) }.description)").probe("widths")
        }
        .onPreferenceChange(WidthKey.self) { w in widths = w; Harness.note("preference \(w)") }
        .frame(width: 360, height: 260)
        .finalState("preference_keys")
    }
}

Harness.run("preference_keys", 360, 260) { V() }

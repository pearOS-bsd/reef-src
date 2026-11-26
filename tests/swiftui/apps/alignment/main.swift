import SwiftUI

extension VerticalAlignment {
    struct Custom: AlignmentID { static func defaultValue(in d: ViewDimensions) -> CGFloat { d[.bottom] } }
    static let custom = VerticalAlignment(Custom.self)
}
struct V: View {
    var body: some View {
        VStack(alignment: .trailing, spacing: 6) {
            HStack(alignment: .firstTextBaseline) {
                Text("Big").font(.system(size: 30))
                Text("small").font(.system(size: 12))
            }.probe("baseline")
            HStack(alignment: .lastTextBaseline) {
                Text("Big").font(.system(size: 30))
                Text("small").font(.system(size: 12))
            }.probe("lastbaseline")
            HStack(alignment: .center) {
                Rectangle().fill(.red).frame(width: 20, height: 40)
                Rectangle().fill(.blue).frame(width: 20, height: 20).alignmentGuide(VerticalAlignment.center) { d in d[.top] }
            }.probe("guide")
            HStack(alignment: .custom) {
                Rectangle().fill(.green).frame(width: 20, height: 40).alignmentGuide(.custom) { d in d[.top] + 5 }
                Rectangle().fill(.orange).frame(width: 20, height: 20).alignmentGuide(.custom) { d in d[.bottom] }
            }.probe("custom")
            VStack(alignment: .leading) {
                Text("left").probe("l1")
                Text("a bit longer").probe("l2")
            }.probe("leading")
            ZStack(alignment: .bottomLeading) {
                Rectangle().fill(.gray).frame(width: 80, height: 50)
                Text("zs")
            }.probe("zstack")
        }
        .padding(10).probe("root")
        .frame(width: 360, height: 300)
        .finalState("alignment")
    }
}

Harness.run("alignment", 360, 300) { V() }

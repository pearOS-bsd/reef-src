import SwiftUI

struct V: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text("Large Title").font(.largeTitle).probe("t1")
            Text("Title").font(.title).probe("t2")
            Text("Headline").font(.headline).probe("t3")
            Text("Body").font(.body).probe("t4")
            Text("Caption").font(.caption).probe("t5")
            Text("Bold 20").font(.system(size: 20, weight: .bold)).probe("t6")
            Text("Italic light").italic().fontWeight(.light).probe("t7")
            Text("Monospaced 14").font(.system(size: 14, design: .monospaced)).probe("t8")
            Text("Serif").font(.system(size: 16, design: .serif)).probe("t9")
            Text("Rounded").font(.system(size: 16, design: .rounded)).probe("t10")
            Text("Colored").foregroundColor(.red).probe("t11")
            Text("Underline").underline().probe("t12")
            Text("Strikethrough").strikethrough(true, color: .blue).probe("t13")
            Text("Kerned").kerning(3).probe("t14")
            Text("Tracked").tracking(2).probe("t15")
            Text("Baseline").baselineOffset(5).probe("t16")
            (Text("Mixed ") + Text("bold").bold() + Text(" red").foregroundColor(.red)).probe("t17")
            Text("Line spacing\nsecond line").lineSpacing(10).probe("t18")
            Text(verbatim: "Verbatim **not markdown**").probe("t19")
            Text("Markdown **bold** and *italic*").probe("t20")
            Text(AttributedString("Attributed")).probe("t21")
            Text("\(42) interpolated \(3.5, specifier: "%.2f")").probe("t22")
        }
        .padding(10)
        .frame(width: 420, height: 400, alignment: .topLeading)
        .finalState("text_styles")
    }
}

Harness.run("text_styles", 420, 400) { V() }

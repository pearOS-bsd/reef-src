import SwiftUI

struct V: View {
    let long = "The quick brown fox jumps over the lazy dog and keeps running through the green field until the sun goes down."
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text(long).frame(width: 200).probe("wrap")
            Text(long).lineLimit(2).frame(width: 200).probe("limit2")
            Text(long).lineLimit(1).truncationMode(.middle).frame(width: 200).probe("middle")
            Text(long).lineLimit(1).truncationMode(.head).frame(width: 200).probe("head")
            Text(long).lineLimit(3).multilineTextAlignment(.trailing).frame(width: 200).probe("trailing")
            Text(long).lineLimit(3).multilineTextAlignment(.center).frame(width: 200).probe("center")
            Text(long).lineLimit(1).minimumScaleFactor(0.5).frame(width: 200).probe("scale")
            Text("a b c").lineLimit(1).frame(width: 10).probe("tiny")
            Text("").probe("empty")
            Text("Wide\nTwo lines").fixedSize().probe("newline")
            Text(long).lineLimit(2...3).frame(width: 150).probe("range")
        }
        .padding(8)
        .frame(width: 400, height: 400, alignment: .topLeading)
        .finalState("text_wrap")
    }
}

Harness.run("text_wrap", 400, 400) { V() }

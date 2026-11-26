import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            Color.red.aspectRatio(2, contentMode: .fit).frame(width: 100, height: 100).probe("fit")
            Color.green.aspectRatio(2, contentMode: .fill).frame(width: 100, height: 100).clipped().probe("fill")
            Color.blue.aspectRatio(CGSize(width: 1, height: 3), contentMode: .fit).frame(height: 60).probe("size")
            Color.orange.aspectRatio(1, contentMode: .fit).frame(maxWidth: .infinity, maxHeight: 40).probe("flex")
            Rectangle().fill(.purple).aspectRatio(contentMode: .fit).frame(width: 50, height: 20).probe("noratio")
            Text("text ratio").aspectRatio(3, contentMode: .fit).probe("text")
        }
        .frame(width: 400, height: 300)
        .finalState("aspectratio")
    }
}

Harness.run("aspectratio", 400, 300) { V() }

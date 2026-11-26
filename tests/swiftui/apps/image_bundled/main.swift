import SwiftUI

struct V: View {
    let img = Bundle.main.image(forResource: "tile")!
    var body: some View {
        VStack(spacing: 6) {
            HStack(spacing: 6) {
                Image("tile").probe("plain")
                Image(nsImage: img).resizable().frame(width: 40, height: 20).probe("stretch")
                Image("tile").resizable().scaledToFit().frame(width: 60, height: 30).probe("fit")
                Image("tile").resizable().scaledToFill().frame(width: 40, height: 40).clipped().probe("fill")
            }
            HStack(spacing: 6) {
                Image("tile").resizable().interpolation(.none).frame(width: 64, height: 64).probe("nearest")
                Image("tile").renderingMode(.template).foregroundColor(.red).probe("template")
                Image("tile").opacity(0.5).probe("opacity")
                Image("tile").resizable(resizingMode: .tile).frame(width: 64, height: 40).probe("tiled")
                Image("tile").clipShape(Circle()).probe("circle")
            }
            Image("nonexistent-image").probe("nonexistent")
            Image(nsImage: img).rotationEffect(.degrees(90)).probe("rotated")
        }
        .frame(width: 400, height: 360)
        .finalState("image_bundled")
    }
}

Harness.run("image_bundled", 400, 360) { V() }

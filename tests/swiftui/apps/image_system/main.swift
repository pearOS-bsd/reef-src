import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            HStack(spacing: 8) {
                Image(systemName: "star.fill").probe("star")
                Image(systemName: "heart").font(.system(size: 30)).probe("heart")
                Image(systemName: "gear").imageScale(.large).probe("gear")
                Image(systemName: "house").foregroundColor(.red).probe("house")
                Image(systemName: "folder").resizable().scaledToFit().frame(width: 40, height: 40).probe("folder")
            }
            HStack(spacing: 8) {
                Image(systemName: "sun.max").symbolRenderingMode(.multicolor).font(.largeTitle).probe("sun")
                Image(systemName: "cloud.sun.fill").symbolRenderingMode(.palette).foregroundStyle(.gray, .yellow).font(.largeTitle).probe("cloudsun")
                Image(systemName: "bolt.fill").symbolRenderingMode(.hierarchical).foregroundStyle(.blue).font(.largeTitle).probe("bolt")
                Image(systemName: "this.symbol.does.not.exist").probe("missing")
            }
            Label("Label with icon", systemImage: "tag").probe("label")
            Label { Text("Title only") } icon: { Image(systemName: "pencil") }.labelStyle(.titleOnly).probe("titleonly")
            Label("Icon only", systemImage: "trash").labelStyle(.iconOnly).probe("icononly")
        }
        .frame(width: 360, height: 300)
        .finalState("image_system")
    }
}

Harness.run("image_system", 360, 300) { V() }

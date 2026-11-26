import SwiftUI

struct V: View {
    @SceneStorage("pear.test.scene") var scene = "scene-default"
    var body: some View {
        Text("scene \(scene)").probe("values").frame(width: 360, height: 120)
            .onAppear { scene = "changed" }
            .finalState("scenestorage")
    }
}

Harness.run("scenestorage", 360, 120) { V() }

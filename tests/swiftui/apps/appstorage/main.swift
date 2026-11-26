import SwiftUI

struct V: View {
    @AppStorage("pear.test.counter") var counter = 0
    @AppStorage("pear.test.name") var name = "default"
    @SceneStorage("pear.test.scene") var scene = "scene-default"
    var body: some View {
        VStack(spacing: 6) {
            Text("counter \(counter) name \(name) scene \(scene)").probe("values")
        }
        .frame(width: 360, height: 200)
        .onAppear { counter = 5; name = "stored"; Harness.note("defaults \(UserDefaults.standard.integer(forKey: "pear.test.counter"))") }
        .finalState("appstorage")
    }
}

Harness.run("appstorage", 360, 200) { V() }

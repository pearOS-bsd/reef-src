import SwiftUI

struct V: View {
    @AppStorage("pear.test.counter") var counter = 0
    @AppStorage("pear.test.name") var name = "default"
    var body: some View {
        Text("counter \(counter) name \(name)").probe("values").frame(width: 360, height: 120)
            .onAppear { counter = 5; name = "stored"; Harness.note("defaults \(UserDefaults.standard.integer(forKey: "pear.test.counter"))") }
            .finalState("appstorage")
    }
}

Harness.run("appstorage", 360, 120) { V() }

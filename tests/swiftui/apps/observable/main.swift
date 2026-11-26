import SwiftUI

import Observation
@Observable final class Model {
    var count = 0
    var name = "model"
    var items: [String] = []
    var ignoredValue = 0
    func bump() { count += 1; items.append("i\(count)") }
}
struct Reader: View {
    var model: Model
    var body: some View { Text("reader \(model.count) \(model.name) \(model.items.count)").probe("reader") }
}
struct Writer: View {
    @Bindable var model: Model
    var body: some View { TextField("name", text: $model.name).frame(width: 150).probe("writer") }
}
struct V: View {
    @State var model = Model()
    var body: some View {
        VStack(spacing: 8) {
            Reader(model: model)
            Writer(model: model)
            Text("items \(model.items.joined(separator: ","))").probe("items")
        }
        .frame(width: 360, height: 260)
        .onAppear { model.bump(); DispatchQueue.main.async { model.bump(); model.name = "renamed"; DispatchQueue.main.async { model.bump() } } }
        .onChange(of: model.count) { _, v in Harness.note("count \(v)") }
        .finalState("observable")
    }
}

Harness.run("observable", 360, 260) { V() }

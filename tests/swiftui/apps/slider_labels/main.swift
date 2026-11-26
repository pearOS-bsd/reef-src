import SwiftUI

struct V: View {
    @State var stepped = 4.0
    var body: some View {
        Slider(value: $stepped, in: 0...10, step: 2) { Text("Stepped") } minimumValueLabel: { Text("0") } maximumValueLabel: { Text("10") }.probe("labels")
            .padding().frame(width: 420, height: 200)
            .finalState("slider_labels")
    }
}

Harness.run("slider_labels", 420, 200) { V() }

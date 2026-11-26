import SwiftUI

struct V: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            ProgressView().probe("spinner")
            ProgressView("Loading", value: 0.4).probe("linear")
            ProgressView(value: 3, total: 10).progressViewStyle(.linear).probe("total")
            ProgressView().progressViewStyle(.circular).controlSize(.small).probe("small")
            Gauge(value: 0.6) { Text("Gauge") } currentValueLabel: { Text("60") }.probe("gauge")
            Gauge(value: 0.3) { Text("Linear") }.gaugeStyle(.linearCapacity).probe("capacity")
            Gauge(value: 5, in: 0...10) { Text("Circular") }.gaugeStyle(.accessoryCircular).probe("circular")
            ProgressView(value: 1.0).tint(.green).probe("full")
        }
        .padding()
        .frame(width: 420, height: 300, alignment: .topLeading)
        .finalState("progress_gauge")
    }
}

Harness.run("progress_gauge", 420, 300) { V() }

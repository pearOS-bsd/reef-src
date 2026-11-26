import SwiftUI

struct V: View {
    @State var v = 0.25
    @State var stepped = 4.0
    @State var step = 3
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Slider(value: $v).probe("slider")
            Slider(value: $v, in: 0...10).frame(width: 200).probe("range")
            Slider(value: $stepped, in: 0...10, step: 2) { Text("Stepped") } minimumValueLabel: { Text("0") } maximumValueLabel: { Text("10") }.probe("labels")
            Slider(value: $v).disabled(true).probe("disabled")
            Stepper("Step: \(step)", value: $step, in: 0...10).probe("stepper")
            Stepper(value: $step, in: 0...10, step: 2) { Text("By two") }.probe("stepper2")
            ProgressView(value: v).probe("progress")
            Gauge(value: v) { Text("G") }.probe("gauge")
            Text("v=\(v) stepped=\(stepped) step=\(step)").probe("state")
        }
        .padding()
        .frame(width: 420, height: 300, alignment: .topLeading)
        .onAppear { DispatchQueue.main.async { v = 0.75; step = 5 } }
        .finalState("slider")
    }
}

Harness.run("slider", 420, 300) { V() }

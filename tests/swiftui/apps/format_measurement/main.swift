import SwiftUI

struct V: View {
    var body: some View {
        Text(Measurement(value: 5, unit: UnitLength.kilometers), format: .measurement(width: .abbreviated)).environment(\.locale, Locale(identifier: "en_US")).probe("measure")
            .frame(width: 360, height: 100)
            .finalState("format_measurement")
    }
}

Harness.run("format_measurement", 360, 100) { V() }

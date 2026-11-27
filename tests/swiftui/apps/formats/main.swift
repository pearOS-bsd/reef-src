import SwiftUI

struct V: View {
    let date = Date(timeIntervalSince1970: 1_700_000_000)
    let en = Locale(identifier: "en_US")
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(date, style: .date).environment(\.locale, en).environment(\.timeZone, TimeZone(identifier: "UTC")!).probe("date")
            Text(date, format: .dateTime.year().month().day()).environment(\.locale, en).environment(\.timeZone, TimeZone(identifier: "UTC")!).probe("fmt")
            Text(1234.5, format: .number.precision(.fractionLength(1))).environment(\.locale, en).probe("number")
            Text(0.256, format: .percent).environment(\.locale, en).probe("percent")
            Text("\(3) items").probe("interp")
            Text(Image(systemName: "star")).probe("inlineimage")
        }
        .padding()
        .frame(width: 420, height: 220, alignment: .topLeading)
        .finalState("formats")
    }
}

Harness.run("formats", 420, 220) { V() }

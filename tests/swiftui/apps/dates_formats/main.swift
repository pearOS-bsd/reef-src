import SwiftUI

struct V: View {
    let date = Date(timeIntervalSince1970: 1_700_000_000)
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(date, style: .date).environment(\.locale, Locale(identifier: "en_US")).environment(\.timeZone, TimeZone(identifier: "UTC")!).probe("date")
            Text(date, format: .dateTime.year().month().day()).environment(\.locale, Locale(identifier: "en_US")).environment(\.timeZone, TimeZone(identifier: "UTC")!).probe("fmt")
            Text(1234.5, format: .number.precision(.fractionLength(1))).environment(\.locale, Locale(identifier: "en_US")).probe("number")
            Text(0.256, format: .percent).environment(\.locale, Locale(identifier: "en_US")).probe("percent")
            Text(12.5, format: .currency(code: "USD")).environment(\.locale, Locale(identifier: "en_US")).probe("currency")
            Text(Measurement(value: 5, unit: UnitLength.kilometers), format: .measurement(width: .abbreviated)).environment(\.locale, Locale(identifier: "en_US")).probe("measure")
            Text("\(3) items", tableName: nil).probe("interp")
            Text(Image(systemName: "star") ).probe("inlineimage")
            DatePicker("Date", selection: .constant(date), displayedComponents: .date).environment(\.timeZone, TimeZone(identifier: "UTC")!).environment(\.locale, Locale(identifier: "en_US")).probe("picker")
            ColorPicker("Color", selection: .constant(Color.red)).probe("colorpicker")
        }
        .padding()
        .frame(width: 420, height: 300, alignment: .topLeading)
        .finalState("dates_formats")
    }
}

Harness.run("dates_formats", 420, 300) { V() }

import SwiftUI

struct V: View {
    let date = Date(timeIntervalSince1970: 1_700_000_000)
    var body: some View {
        VStack(alignment: .leading) {
            DatePicker("Date", selection: .constant(date), displayedComponents: .date).environment(\.timeZone, TimeZone(identifier: "UTC")!).environment(\.locale, Locale(identifier: "en_US")).probe("datepicker")
            ColorPicker("Color", selection: .constant(Color.red)).probe("colorpicker")
        }
        .padding()
        .frame(width: 420, height: 140, alignment: .topLeading)
        .finalState("pickers_date_color")
    }
}

Harness.run("pickers_date_color", 420, 140) { V() }

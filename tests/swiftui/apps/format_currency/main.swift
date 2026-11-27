import SwiftUI

struct V: View {
    var body: some View {
        Text(12.5, format: .currency(code: "USD")).environment(\.locale, Locale(identifier: "en_US")).probe("currency")
            .frame(width: 360, height: 100)
            .finalState("format_currency")
    }
}

Harness.run("format_currency", 360, 100) { V() }

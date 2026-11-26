import SwiftUI

struct V: View {
    @State var name = "Name"
    @State var on = true
    @State var expanded = true
    var body: some View {
        Form {
            Section("Section header") {
                TextField("Name", text: $name)
                Toggle("Enabled", isOn: $on)
                LabeledContent("Labeled", value: "value")
                LabeledContent("Custom") { Text("trailing") }
            }
            Section { Text("Footerless section") } header: { Text("Custom header") } footer: { Text("Footer text") }
            GroupBox("Group box") { Text("Inside group") }
            GroupBox { Text("Unlabeled") }
            DisclosureGroup("Disclosure", isExpanded: $expanded) { Text("Hidden content") }
        }
        .formStyle(.grouped)
        .probe("form")
        .frame(width: 460, height: 420)
        .onAppear { DispatchQueue.main.async { expanded = false } }
        .finalState("form_groupbox")
    }
}

Harness.run("form_groupbox", 460, 420) { V() }

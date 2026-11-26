import SwiftUI

import AppKit
final class Box: NSView {
    var tint = NSColor.red { didSet { needsDisplay = true } }
    override var intrinsicContentSize: NSSize { NSSize(width: 80, height: 40) }
    override func draw(_ r: NSRect) { tint.setFill(); r.fill() }
}
struct BoxView: NSViewRepresentable {
    var color: NSColor
    var onMake: () -> Void = { }
    func makeNSView(context: Context) -> Box { Harness.note("makeNSView"); return Box() }
    func updateNSView(_ v: Box, context: Context) { Harness.note("updateNSView"); v.tint = color }
    func makeCoordinator() -> Coordinator { Coordinator() }
    final class Coordinator { var n = 0 }
    func sizeThatFits(_ p: ProposedViewSize, nsView: Box, context: Context) -> CGSize? { nil }
}
struct FieldRep: NSViewRepresentable {
    @Binding var text: String
    func makeNSView(context: Context) -> NSTextField { NSTextField(string: text) }
    func updateNSView(_ v: NSTextField, context: Context) { v.stringValue = text }
}
struct V: View {
    @State var color = NSColor.red
    @State var text = "native"
    var body: some View {
        VStack(spacing: 8) {
            BoxView(color: color).probe("box")
            BoxView(color: .green).frame(width: 120, height: 60).probe("boxframed")
            FieldRep(text: $text).frame(width: 150).probe("field")
            HStack { Text("beside"); BoxView(color: .blue) }.probe("beside")
            Text("text=\(text)").probe("state")
        }
        .frame(width: 420, height: 300)
        .onAppear { DispatchQueue.main.async { color = .orange; text = "changed" } }
        .finalState("nsview_representable")
    }
}

Harness.run("nsview_representable", 420, 300) { V() }

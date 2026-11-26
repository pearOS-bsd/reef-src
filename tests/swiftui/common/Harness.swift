// Shared by every test app: marks the final state, dumps the AppKit view tree and exits.
// Test apps stay deterministic: no network, no clock text; state changes come from onAppear chains.
import SwiftUI
import AppKit

enum Harness {
    static var finished = false
    static var started = false
    static var name = ""
    static var records: [String: String] = [:]
    static var notes: [String] = []

    /// Record a value for the final report ("VALUE key=...").
    static func note(_ s: String) { notes.append(s); line("NOTE " + s) }
    static func fmt(_ r: CGRect) -> String { "(\(r.origin.x),\(r.origin.y),\(r.size.width),\(r.size.height))" }

    static func line(_ s: String) {
        FileHandle.standardOutput.write((s + "\n").data(using: .utf8)!)
    }

    static func dumpView(_ v: NSView, _ depth: Int, _ out: inout [String]) {
        let f = v.frame
        var extra = ""
        if let t = v as? NSTextField { extra = " text=\(t.stringValue.debugDescription)" }
        if let b = v as? NSButton { extra = " title=\(b.title.debugDescription)" }
        out.append(String(repeating: "  ", count: depth) + "\(String(describing: type(of: v))) frame=(\(f.origin.x),\(f.origin.y),\(f.size.width),\(f.size.height)) hidden=\(v.isHidden)" + extra)
        for s in v.subviews { dumpView(s, depth + 1, &out) }
    }

    // Called by .finalState(): waits a moment for layout to settle, prints the tree and the final marker.
    static func finish(_ n: String, extra: [String]) {
        if finished { return }
        finished = true
        name = n
        DispatchQueue.main.asyncAfter(deadline: .now() + 1.5) {
            for e in extra { line(e) }
            for k in records.keys.sorted() { line("LAYOUT \(k) \(records[k]!)") }
            var out: [String] = []
            for w in NSApp.windows where w.isVisible || w.contentView != nil {
                let f = w.frame
                out.append("WINDOW \(String(describing: type(of: w))) title=\(w.title.debugDescription) size=(\(f.size.width),\(f.size.height))")
                if let c = w.contentView { dumpView(c, 1, &out) }
            }
            line("VIEWTREE-BEGIN")
            for l in out { line(l) }
            line("VIEWTREE-END")
            line("FINAL \(n)")
        }
    }
}

/// Window host: an AppKit window with an NSHostingView (WindowGroup { } needs the non-escaping content
/// initialisers, which Reef's SwiftUI does not export, so apps built here start through AppKit instead;
/// the `windowgroup` app tests the scene API on its own).
final class HarnessDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow?
    var make: (() -> NSView)!
    var title = ""
    var size = CGSize(width: 400, height: 300)
    func applicationDidFinishLaunching(_ n: Notification) {
        let w = NSWindow(contentRect: NSRect(x: 100, y: 100, width: size.width, height: size.height),
                         styleMask: [.titled, .closable, .resizable, .miniaturizable], backing: .buffered, defer: false)
        w.title = title
        w.contentView = make()
        w.center()
        w.makeKeyAndOrderFront(nil)
        window = w
        NSApp.activate(ignoringOtherApps: true)
    }
    func applicationShouldTerminateAfterLastWindowClosed(_ s: NSApplication) -> Bool { true }
}

extension Harness {
    static var delegate: HarnessDelegate?
    /// Start the app: one window of `width` x `height` hosting `content`. Never returns.
    static func run<V: View>(_ title: String, _ width: CGFloat, _ height: CGFloat, @ViewBuilder content: @escaping () -> V) -> Never {
        let d = HarnessDelegate()
        d.title = title
        d.size = CGSize(width: width, height: height)
        d.make = { NSHostingView(rootView: content()) }
        delegate = d
        let app = NSApplication.shared
        app.setActivationPolicy(.regular)
        app.delegate = d
        app.run()
        exit(0)
    }
}

extension View {
    /// Report the frame (global coordinates of the window content) of this view in the final LAYOUT lines.
    func probe(_ name: String) -> some View {
        self.background(GeometryReader { g in
            Color.clear
                .onAppear { Harness.records[name] = Harness.fmt(g.frame(in: .global)) }
                .onChange(of: g.size) { _ in Harness.records[name] = Harness.fmt(g.frame(in: .global)) }
        })
    }
    /// Mark the view whose appearance means "final state reached". `log` lines are printed with the tree.
    func finalState(_ name: String) -> some View {
        self.onAppear { Harness.finish(name, extra: []) }
    }
}

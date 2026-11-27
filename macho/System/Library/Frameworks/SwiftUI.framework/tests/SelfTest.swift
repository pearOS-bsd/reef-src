// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
// Test-only entry compiled into the module (not part of Sources) by tools/selftest.sh: lays out a few elements and
// prints their frames, reads the environment and a State.
import Foundation
import CoreGraphics
import AppKit

@_cdecl("swiftui_selftest")
public func swiftui_selftest() {
    let a = SwiftUI.FrameElement(SwiftUI.Element(), width: 50, height: 20, alignment: .center)
    let s = SwiftUI.SpacerElement(minLength: nil)
    let b = SwiftUI.FrameElement(SwiftUI.Element(), width: 30, height: 40, alignment: .center)
    let h = SwiftUI.StackElement(axis: .horizontal, alignment: SwiftUI.VerticalAlignment.center.key, spacing: nil, items: [a, s, b])
    print("hstack measure", h.sizeThatFits(SwiftUI.ProposedViewSize(width: 300, height: 100)))
    h.place(in: CGRect(x: 0, y: 0, width: 300, height: 100))
    for e in [a, s, b] { print(" ", e.frame) }
    let p = SwiftUI.PaddingElement(SwiftUI.FrameElement(SwiftUI.Element(), width: 10, height: 10, alignment: .center), insets: SwiftUI.EdgeInsets(_all: 5))
    let v = SwiftUI.StackElement(axis: .vertical, alignment: SwiftUI.HorizontalAlignment.leading.key, spacing: 4, items: [p, SwiftUI.FrameElement(SwiftUI.Element(), width: 40, height: 10, alignment: .center)])
    print("vstack measure", v.sizeThatFits(.unspecified))
    v.place(in: CGRect(x: 10, y: 10, width: 100, height: 100))
    for e in v.items { print(" ", e.frame) }
    let f = SwiftUI.FlexFrameElement(SwiftUI.FrameElement(SwiftUI.Element(), width: 20, height: 20, alignment: .center),
        layout: SwiftUI._FlexFrameLayout(minWidth: nil, idealWidth: nil, maxWidth: .infinity, minHeight: nil, idealHeight: nil, maxHeight: nil, alignment: .center))
    print("flex inf", f.sizeThatFits(SwiftUI.ProposedViewSize(width: .infinity, height: 50)), "nil", f.sizeThatFits(.unspecified))
    var env = SwiftUI.EnvironmentValues()
    print("env", env.isEnabled, env.displayScale, env.lineLimit as Any, env.colorScheme)
    env.isEnabled = false
    print("env set", env.isEnabled)
    let st = SwiftUI.State(wrappedValue: 3)
    print("state", st.wrappedValue, st.projectedValue.wrappedValue)
    selftestBuild()
}

/// a primitive view of the test: one fixed-size element
struct SelfTestLeaf: SwiftUI.View, SwiftUI.PrimitiveView {
    typealias Body = Never
    var body: Never { fatalError() }
    let width: CGFloat
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        [SwiftUI.FrameElement(SwiftUI.Element(), width: width, height: 10, alignment: .center)]
    }
}

/// a view with state whose body is a leaf as wide as the state
struct SelfTestCounter: SwiftUI.View {
    @SwiftUI.State var count = 7
    var body: SelfTestLeaf { SelfTestLeaf(width: CGFloat(count)) }
}

/// ViewBuild: a body built under its node, reused in the next generation, built again after a State change
func selftestBuild() {
    let host = SwiftUI.ViewHost(rootType: SelfTestCounter.self)
    func context() -> SwiftUI.BuildContext {
        SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(), canvas: nil,
            modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    }
    host.generation = 1
    let first = SwiftUI.ViewBuild.build(SelfTestCounter(), context())
    let node = host.root.children.values.first!
    print("build", first.count, first[0].sizeThatFits(.unspecified), "runs", node.bodyRuns, "entries",
          SwiftUI.DynamicPropertyLayout.entries(of: SelfTestCounter.self).count)
    host.generation = 2
    let second = SwiftUI.ViewBuild.build(SelfTestCounter(), context())
    print("reuse", second[0] === first[0], "runs", node.bodyRuns)
    (node.storage.values.first as! SwiftUI.StoredLocation<Int>).set(12, transaction: SwiftUI.Transaction())
    print("dirty", node.isDirty, node.needsBuild, host.needsUpdate)
    host.generation = 3
    let third = SwiftUI.ViewBuild.build(SelfTestCounter(), context())
    print("rebuild", third[0] === first[0], third[0].sizeThatFits(.unspecified), "runs", node.bodyRuns)
    selftestStacks(host)
}

/// a body of stacks, a spacer, a condition and a tuple
struct SelfTestStacks: SwiftUI.View {
    var wide: Bool
    @ViewBuilder var body: some SwiftUI.View {
        SwiftUI.VStack(alignment: .leading, spacing: 4) {
            SwiftUI.HStack(alignment: .center, spacing: nil) {
                SelfTestLeaf(width: 30)
                SwiftUI.Spacer(minLength: nil)
                SelfTestLeaf(width: 20)
            }
            if wide { SelfTestLeaf(width: 100) } else { SelfTestLeaf(width: 10) }
            SwiftUI.ZStack(alignment: .center) { SelfTestLeaf(width: 5) }
                .padding(3)
                .frame(width: 60, height: nil, alignment: .trailing)
                .environment(\.isEnabled, false)
        }
        .padding(.horizontal, nil)
    }
}

func selftestStacks(_ host: SwiftUI.ViewHost) {
    let context = SwiftUI.BuildContext(host: host, scope: host.root, path: [], environment: SwiftUI.EnvironmentValues(),
        canvas: nil, modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
    for wide in [false, true] {
        let elements = SwiftUI.ViewBuild.build(SelfTestStacks(wide: wide), context)
        let root = elements[0]
        print("stacks", elements.count, type(of: root), root.sizeThatFits(SwiftUI.ProposedViewSize(width: 200, height: 100)))
        root.place(in: CGRect(x: 0, y: 0, width: 200, height: 100))
        for child in root.children {
            print(" ", type(of: child), child.frame, child.children.map { $0.frame })
        }
    }
}

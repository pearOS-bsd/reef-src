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
}

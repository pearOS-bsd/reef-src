// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Button (Controls).

import Foundation
import AppKit
import CoreGraphics
import CoreText
import Combine
import Observation
import CoreTransferable
import UniformTypeIdentifiers
import Symbols
import DeveloperToolsSupport
import Metal
import os

public struct Button<A> where A: SwiftUI.View {
    let action: () -> ()
    let label: A
    let role: SwiftUI.ButtonRole?
    public init(action a0: @escaping () -> (), label a1: () -> A) { _skeleton() }
    init(action a0: @escaping () -> (), label a1: A, role a2: SwiftUI.ButtonRole?) { _skeleton() }
    public init(role a0: SwiftUI.ButtonRole?, action a1: @escaping () -> (), label a2: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}

extension SwiftUI.Button where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, action a1: @escaping () -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, action a1: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, role a1: SwiftUI.ButtonRole?, action a2: @escaping () -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, role a1: SwiftUI.ButtonRole?, action a2: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Button where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, action a2: @escaping () -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, action a2: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, role a2: SwiftUI.ButtonRole?, action a3: @escaping () -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, role a2: SwiftUI.ButtonRole?, action a3: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Button where A == SwiftUI.PrimitiveButtonStyleConfiguration.Label {
    public init(_ a0: SwiftUI.PrimitiveButtonStyleConfiguration) { _skeleton() }
}

extension SwiftUI.Button where A == SwiftUI.DefaultButtonLabel {
    public init(role a0: SwiftUI.ButtonRole, action a1: @escaping () -> ()) { _skeleton() }
}

extension SwiftUI.Button: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Button: SwiftUI.PrimitiveView {
}

extension SwiftUI.Button: SwiftUI.MenuContributor {
}

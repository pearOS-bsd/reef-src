// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Toggle (Controls).

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

public struct Toggle<A> where A: SwiftUI.View {
    let isOn: SwiftUI.Binding<Swift.Bool>
    let label: A
    var isMixed: Swift.Bool
    public init(isOn a0: SwiftUI.Binding<Swift.Bool>, label a1: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    public init<A1>(sources a0: A1, isOn a1: Swift.KeyPath<A1.Element, SwiftUI.Binding<Swift.Bool>>, label a2: () -> A) where A1: Swift.RandomAccessCollection { _skeleton() }
}

extension SwiftUI.Toggle where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, isOn a1: SwiftUI.Binding<Swift.Bool>) { _skeleton() }
    public init<A1>(_ a0: A1, isOn a1: SwiftUI.Binding<Swift.Bool>) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, sources a1: A1, isOn a2: Swift.KeyPath<A1.Element, SwiftUI.Binding<Swift.Bool>>) where A1: Swift.RandomAccessCollection { _skeleton() }
}

extension SwiftUI.Toggle where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, isOn a2: SwiftUI.Binding<Swift.Bool>) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, isOn a2: SwiftUI.Binding<Swift.Bool>) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Toggle where A == SwiftUI.ToggleStyleConfiguration.Label {
    public init(_ a0: SwiftUI.ToggleStyleConfiguration) { _skeleton() }
}

extension SwiftUI.Toggle: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Toggle: SwiftUI.PrimitiveView {
}

extension SwiftUI.Toggle: SwiftUI.MenuContributor {
}

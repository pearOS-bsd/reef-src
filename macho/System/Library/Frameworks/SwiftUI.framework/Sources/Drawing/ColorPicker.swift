// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorPicker (Drawing).

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

public struct ColorPicker<A> where A: SwiftUI.View {
    let selection: SwiftUI.Binding<SwiftUI.Color>
    let supportsOpacity: Swift.Bool
    let label: A
    public init(selection a0: SwiftUI.Binding<SwiftUI.Color>, supportsOpacity a1: Swift.Bool, label a2: () -> A) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<CGColor>, supportsOpacity a1: Swift.Bool, label a2: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ColorPicker where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<SwiftUI.Color>, supportsOpacity a2: Swift.Bool) { _skeleton() }
    public init<A1>(_ a0: A1, selection a1: SwiftUI.Binding<SwiftUI.Color>, supportsOpacity a2: Swift.Bool) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<CGColor>, supportsOpacity a2: Swift.Bool) { _skeleton() }
}

extension SwiftUI.ColorPicker: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ColorPicker: SwiftUI.PrimitiveView {
}

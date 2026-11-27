// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GroupBox (Controls).

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

public struct GroupBox<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let label: A
    let content: B
    init(label a0: A, content a1: B) { _skeleton() }
    public init(content a0: () -> B, label a1: () -> A) { _skeleton() }
    public init(label a0: A, content a1: () -> B) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func custom<A1>(_ a0: A1, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A1: SwiftUI.GroupBoxStyle { _skeleton() }
}

extension SwiftUI.GroupBox where A == SwiftUI.EmptyView {
    public init(content a0: () -> B) { _skeleton() }
}

extension SwiftUI.GroupBox where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> B) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.GroupBox where A == SwiftUI.GroupBoxStyleConfiguration.Label, B == SwiftUI.GroupBoxStyleConfiguration.Content {
    public init(_ a0: SwiftUI.GroupBoxStyleConfiguration) { _skeleton() }
}

extension SwiftUI.GroupBox: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.GroupBox: SwiftUI.PrimitiveView {
}

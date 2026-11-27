// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DisclosureGroup (Controls).

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

public struct DisclosureGroup<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let isExpanded: (SwiftUI.Binding<Swift.Bool>)?
    let label: A
    let content: () -> B
    public init(content a0: @escaping () -> B, label a1: () -> A) { _skeleton() }
    public init(isExpanded a0: SwiftUI.Binding<Swift.Bool>, content a1: @escaping () -> B, label a2: () -> A) { _skeleton() }
    func custom<A1>(_ a0: A1, expanded a1: SwiftUI.Binding<Swift.Bool>, _ a2: SwiftUI.BuildContext) -> [SwiftUI.Element] where A1: SwiftUI.DisclosureGroupStyle { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.DisclosureGroup where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: @escaping () -> B) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, isExpanded a1: SwiftUI.Binding<Swift.Bool>, content a2: @escaping () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: @escaping () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, isExpanded a1: SwiftUI.Binding<Swift.Bool>, content a2: @escaping () -> B) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.DisclosureGroup: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.DisclosureGroup: SwiftUI.PrimitiveView {
}

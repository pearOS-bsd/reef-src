// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProgressView (Controls).

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

public struct ProgressView<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let fractionCompleted: Swift.Double?
    let label: A?
    let currentValueLabel: B?
    init(fractionCompleted a0: Swift.Double?, label a1: A?, currentValueLabel a2: B?) { _skeleton() }
    static func fraction<A1>(_ a0: A1?, _ a1: A1) -> Swift.Double? where A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: A1?, total a1: A1) where A == SwiftUI.EmptyView, B == SwiftUI.EmptyView, A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: A1?, total a1: A1, label a2: () -> A) where B == SwiftUI.EmptyView, A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: A1?, total a1: A1, label a2: () -> A, currentValueLabel a3: () -> B) where A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: A1?, total a2: A1) where A == SwiftUI.Text, B == SwiftUI.EmptyView, A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: B1?, total a2: B1) where A == SwiftUI.Text, B == SwiftUI.EmptyView, A1: Swift.StringProtocol, B1: Swift.BinaryFloatingPoint { _skeleton() }
    public init(_ a0: Progress) where A == SwiftUI.EmptyView, B == SwiftUI.EmptyView { _skeleton() }
    public init(_ a0: SwiftUI.ProgressViewStyleConfiguration) where A == SwiftUI.ProgressViewStyleConfiguration.Label, B == SwiftUI.ProgressViewStyleConfiguration.CurrentValueLabel { _skeleton() }
    func custom<A1>(_ a0: A1, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A1: SwiftUI.ProgressViewStyle { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ProgressView where B == SwiftUI.EmptyView {
    public init() where A == SwiftUI.EmptyView { _skeleton() }
    public init(label a0: () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey) where A == SwiftUI.Text { _skeleton() }
    public init<A1>(_ a0: A1) where A == SwiftUI.Text, A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.ProgressView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ProgressView: SwiftUI.PrimitiveView {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Gauge (Controls).

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

public struct Gauge<A, B, C, D> where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View, D: SwiftUI.View {
    let fraction: Swift.Double
    let label: A
    let currentValueLabel: B?
    let minimumValueLabel: C?
    let maximumValueLabel: C?
    init(fraction a0: Swift.Double, label a1: A, currentValueLabel a2: B?, minimumValueLabel a3: C?, maximumValueLabel a4: C?) { _skeleton() }
    static func fraction<A1>(_ a0: A1, _ a1: Swift.ClosedRange<A1>) -> Swift.Double where A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: A1, `in` a1: Swift.ClosedRange<A1>, label a2: () -> A) where B == SwiftUI.EmptyView, C == SwiftUI.EmptyView, D == SwiftUI.EmptyView, A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: A1, `in` a1: Swift.ClosedRange<A1>, label a2: () -> A, currentValueLabel a3: () -> B) where C == SwiftUI.EmptyView, D == SwiftUI.EmptyView, A1: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: A1, `in` a1: Swift.ClosedRange<A1>, label a2: () -> A, currentValueLabel a3: () -> B, minimumValueLabel a4: () -> C, maximumValueLabel a5: () -> C) where D == SwiftUI.EmptyView, A1: Swift.BinaryFloatingPoint { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Gauge: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Gauge: SwiftUI.PrimitiveView {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Slider (Controls).

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

public struct Slider<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let value: SwiftUI.Binding<Swift.Double>
    let bounds: Swift.ClosedRange<Swift.Double>
    let step: Swift.Double?
    let onEditingChanged: (Swift.Bool) -> ()
    let label: A
    let minimumValueLabel: B?
    let maximumValueLabel: B?
    var tickValues: [Swift.Double]?
    init<A1>(value a0: SwiftUI.Binding<A1>, bounds a1: Swift.ClosedRange<A1>, step a2: A1.Stride?, onEditingChanged a3: @escaping (Swift.Bool) -> (), label a4: A, minimumValueLabel a5: B?, maximumValueLabel a6: B?) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    var fraction: Swift.Double { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, label a2: () -> A, minimumValueLabel a3: () -> B, maximumValueLabel a4: () -> B, onEditingChanged a5: @escaping (Swift.Bool) -> ()) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, step a2: A1.Stride, label a3: () -> A, minimumValueLabel a4: () -> B, maximumValueLabel a5: () -> B, onEditingChanged a6: @escaping (Swift.Bool) -> ()) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init<A1, B1, C1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, neutralValue a2: A1?, enabledBounds a3: (Swift.ClosedRange<A1>)?, label a4: () -> A, currentValueLabel a5: () -> B1, minimumValueLabel a6: () -> B, maximumValueLabel a7: () -> B, ticks a8: () -> C1, onEditingChanged a9: @escaping (Swift.Bool) -> ()) where A1 == C1.Value, B1: SwiftUI.View, C1: SwiftUI.SliderTickContent, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
}

extension SwiftUI.Slider where B == SwiftUI.EmptyView {
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, label a2: () -> A, onEditingChanged a3: @escaping (Swift.Bool) -> ()) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, step a2: A1.Stride, label a3: () -> A, onEditingChanged a4: @escaping (Swift.Bool) -> ()) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, onEditingChanged a2: @escaping (Swift.Bool) -> (), label a3: () -> A) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, step a2: A1.Stride, onEditingChanged a3: @escaping (Swift.Bool) -> (), label a4: () -> A) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
}

extension SwiftUI.Slider where A == SwiftUI.EmptyView, B == SwiftUI.EmptyView {
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, onEditingChanged a2: @escaping (Swift.Bool) -> ()) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, step a2: A1.Stride, onEditingChanged a3: @escaping (Swift.Bool) -> ()) where A1: Swift.BinaryFloatingPoint, A1.Stride: Swift.BinaryFloatingPoint { _skeleton() }
}

extension SwiftUI.Slider: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Slider: SwiftUI.PrimitiveView {
}

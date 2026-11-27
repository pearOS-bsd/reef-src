// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Stepper (Controls).

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

public struct Stepper<A> where A: SwiftUI.View {
    let onIncrement: (() -> ())?
    let onDecrement: (() -> ())?
    let onEditingChanged: (Swift.Bool) -> ()
    let label: A
    let valueText: (() -> Swift.String)?
    var valueField: (SwiftUI.Binding<Swift.String>)?
    init(onIncrement a0: (() -> ())?, onDecrement a1: (() -> ())?, onEditingChanged a2: @escaping (Swift.Bool) -> (), label a3: A, valueText a4: (() -> Swift.String)?) { _skeleton() }
    static func steps<A1>(_ a0: SwiftUI.Binding<A1>, range a1: (Swift.ClosedRange<A1>)?, step a2: A1.Stride) -> (() -> (), () -> ()) where A1: Swift.Strideable { _skeleton() }
    public init(label a0: () -> A, onIncrement a1: (() -> ())?, onDecrement a2: (() -> ())?, onEditingChanged a3: @escaping (Swift.Bool) -> ()) { _skeleton() }
    public init(onIncrement a0: (() -> ())?, onDecrement a1: (() -> ())?, onEditingChanged a2: @escaping (Swift.Bool) -> (), label a3: () -> A) { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, step a1: A1.Stride, onEditingChanged a2: @escaping (Swift.Bool) -> (), label a3: () -> A) where A1: Swift.Strideable { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1>, `in` a1: Swift.ClosedRange<A1>, step a2: A1.Stride, onEditingChanged a3: @escaping (Swift.Bool) -> (), label a4: () -> A) where A1: Swift.Strideable { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1.FormatInput>, step a1: A1.FormatInput.Stride, format a2: A1, label a3: () -> A, onEditingChanged a4: @escaping (Swift.Bool) -> ()) where A1: Foundation.ParseableFormatStyle, A1.FormatInput: Swift.BinaryFloatingPoint, A1.FormatOutput == Swift.String { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1.FormatInput>, `in` a1: Swift.ClosedRange<A1.FormatInput>, step a2: A1.FormatInput.Stride, format a3: A1, label a4: () -> A, onEditingChanged a5: @escaping (Swift.Bool) -> ()) where A1: Foundation.ParseableFormatStyle, A1.FormatInput: Swift.BinaryFloatingPoint, A1.FormatOutput == Swift.String { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Stepper where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, onIncrement a1: (() -> ())?, onDecrement a2: (() -> ())?, onEditingChanged a3: @escaping (Swift.Bool) -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, onIncrement a1: (() -> ())?, onDecrement a2: (() -> ())?, onEditingChanged a3: @escaping (Swift.Bool) -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1>, step a2: A1.Stride, onEditingChanged a3: @escaping (Swift.Bool) -> ()) where A1: Swift.Strideable { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: SwiftUI.Binding<B1>, step a2: B1.Stride, onEditingChanged a3: @escaping (Swift.Bool) -> ()) where A1: Swift.StringProtocol, B1: Swift.Strideable { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1>, `in` a2: Swift.ClosedRange<A1>, step a3: A1.Stride, onEditingChanged a4: @escaping (Swift.Bool) -> ()) where A1: Swift.Strideable { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: SwiftUI.Binding<B1>, `in` a2: Swift.ClosedRange<B1>, step a3: B1.Stride, onEditingChanged a4: @escaping (Swift.Bool) -> ()) where A1: Swift.StringProtocol, B1: Swift.Strideable { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1.FormatInput>, step a2: A1.FormatInput.Stride, format a3: A1, onEditingChanged a4: @escaping (Swift.Bool) -> ()) where A1: Foundation.ParseableFormatStyle, A1.FormatInput: Swift.BinaryFloatingPoint, A1.FormatOutput == Swift.String { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1.FormatInput>, `in` a2: Swift.ClosedRange<A1.FormatInput>, step a3: A1.FormatInput.Stride, format a4: A1, onEditingChanged a5: @escaping (Swift.Bool) -> ()) where A1: Foundation.ParseableFormatStyle, A1.FormatInput: Swift.BinaryFloatingPoint, A1.FormatOutput == Swift.String { _skeleton() }
}

extension SwiftUI.Stepper: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Stepper: SwiftUI.PrimitiveView {
}

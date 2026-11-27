// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DatePicker (Controls).

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

public struct DatePicker<A> where A: SwiftUI.View {
    let selection: SwiftUI.Binding<Foundation.Date>
    let minimum: Foundation.Date?
    let maximum: Foundation.Date?
    let components: SwiftUI.DatePickerComponents
    let label: A
    init(selection a0: SwiftUI.Binding<Foundation.Date>, minimum a1: Foundation.Date?, maximum a2: Foundation.Date?, components a3: SwiftUI.DatePickerComponents, label a4: A) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<Foundation.Date>, displayedComponents a1: SwiftUI.DatePickerComponents, label a2: () -> A) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<Foundation.Date>, `in` a1: Swift.ClosedRange<Foundation.Date>, displayedComponents a2: SwiftUI.DatePickerComponents, label a3: () -> A) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<Foundation.Date>, `in` a1: Swift.PartialRangeFrom<Foundation.Date>, displayedComponents a2: SwiftUI.DatePickerComponents, label a3: () -> A) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<Foundation.Date>, `in` a1: Swift.PartialRangeThrough<Foundation.Date>, displayedComponents a2: SwiftUI.DatePickerComponents, label a3: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.DatePicker where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<Foundation.Date>, displayedComponents a2: SwiftUI.DatePickerComponents) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<Foundation.Date>, `in` a2: Swift.ClosedRange<Foundation.Date>, displayedComponents a3: SwiftUI.DatePickerComponents) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<Foundation.Date>, `in` a2: Swift.PartialRangeFrom<Foundation.Date>, displayedComponents a3: SwiftUI.DatePickerComponents) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<Foundation.Date>, `in` a2: Swift.PartialRangeThrough<Foundation.Date>, displayedComponents a3: SwiftUI.DatePickerComponents) { _skeleton() }
    public init<A1>(_ a0: A1, selection a1: SwiftUI.Binding<Foundation.Date>, displayedComponents a2: SwiftUI.DatePickerComponents) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, selection a1: SwiftUI.Binding<Foundation.Date>, `in` a2: Swift.ClosedRange<Foundation.Date>, displayedComponents a3: SwiftUI.DatePickerComponents) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.DatePicker: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.DatePicker: SwiftUI.PrimitiveView {
}

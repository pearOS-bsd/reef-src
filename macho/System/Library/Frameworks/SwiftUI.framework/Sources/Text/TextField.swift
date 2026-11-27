// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextField (Text).

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

public struct TextField<A> where A: SwiftUI.View {
    let label: A
    let text: (SwiftUI.Binding<Swift.String>)?
    let prompt: SwiftUI.Text?
    let onCommit: () -> ()
    let axis: SwiftUI.Axis
    let isSecure: Swift.Bool
    let selection: (SwiftUI.Binding<SwiftUI.TextSelection?>)?
    static func formatted<A1>(_ a0: SwiftUI.Binding<A1.FormatInput>, _ a1: A1) -> SwiftUI.Binding<Swift.String> where A1: Foundation.ParseableFormatStyle, A1.FormatOutput == Swift.String { _skeleton() }
    init(label a0: A, text a1: (SwiftUI.Binding<Swift.String>)?, prompt a2: SwiftUI.Text?, onCommit a3: @escaping () -> (), axis a4: SwiftUI.Axis, isSecure a5: Swift.Bool, selection a6: (SwiftUI.Binding<SwiftUI.TextSelection?>)?) { _skeleton() }
    static func formatted<A1>(_ a0: SwiftUI.Binding<A1.FormatInput?>, _ a1: A1) -> SwiftUI.Binding<Swift.String> where A1: Foundation.ParseableFormatStyle, A1.FormatOutput == Swift.String { _skeleton() }
    public init(text a0: SwiftUI.Binding<Swift.String>, prompt a1: SwiftUI.Text?, label a2: () -> A) { _skeleton() }
    public init(text a0: SwiftUI.Binding<Swift.String>, prompt a1: SwiftUI.Text?, axis a2: SwiftUI.Axis, label a3: () -> A) { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1.FormatInput>, format a1: A1, prompt a2: SwiftUI.Text?, label a3: () -> A) where A1: Foundation.ParseableFormatStyle, A1.FormatOutput == Swift.String { _skeleton() }
    public init<A1>(value a0: SwiftUI.Binding<A1.FormatInput?>, format a1: A1, prompt a2: SwiftUI.Text?, label a3: () -> A) where A1: Foundation.ParseableFormatStyle, A1.FormatOutput == Swift.String { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(text a0: SwiftUI.Binding<Swift.String>, selection a1: SwiftUI.Binding<SwiftUI.TextSelection?>, prompt a2: SwiftUI.Text?, axis a3: SwiftUI.Axis?, label a4: () -> A) { _skeleton() }
}

extension SwiftUI.TextField where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, axis a2: SwiftUI.Axis) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, axis a2: SwiftUI.Axis) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?, axis a3: SwiftUI.Axis) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?, axis a3: SwiftUI.Axis) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, onEditingChanged a2: @escaping (Swift.Bool) -> (), onCommit a3: @escaping () -> ()) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, onEditingChanged a2: @escaping (Swift.Bool) -> ()) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, onCommit a2: @escaping () -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, onEditingChanged a2: @escaping (Swift.Bool) -> (), onCommit a3: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, onEditingChanged a2: @escaping (Swift.Bool) -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, onCommit a2: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1>, formatter a2: Formatter, onEditingChanged a3: @escaping (Swift.Bool) -> (), onCommit a4: @escaping () -> ()) { _skeleton() }
    static func formatted<A1>(_ a0: SwiftUI.Binding<A1>, _ a1: Formatter) -> SwiftUI.Binding<Swift.String> { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: SwiftUI.Binding<B1>, formatter a2: Formatter, onEditingChanged a3: @escaping (Swift.Bool) -> (), onCommit a4: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1>, formatter a2: Formatter, prompt a3: SwiftUI.Text?) { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1.FormatInput>, format a2: A1, prompt a3: SwiftUI.Text?) where A1: Foundation.ParseableFormatStyle, A1.FormatOutput == Swift.String { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: SwiftUI.Binding<B1.FormatInput>, format a2: B1, prompt a3: SwiftUI.Text?) where A1: Swift.StringProtocol, B1: Foundation.ParseableFormatStyle, B1.FormatOutput == Swift.String { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: SwiftUI.Binding<A1.FormatInput?>, format a2: A1, prompt a3: SwiftUI.Text?) where A1: Foundation.ParseableFormatStyle, A1.FormatOutput == Swift.String { _skeleton() }
    public init(_ a0: Foundation.LocalizedStringResource, text a1: SwiftUI.Binding<Swift.String>) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, selection a2: SwiftUI.Binding<SwiftUI.TextSelection?>, prompt a3: SwiftUI.Text?, axis a4: SwiftUI.Axis?) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, selection a2: SwiftUI.Binding<SwiftUI.TextSelection?>, prompt a3: SwiftUI.Text?, axis a4: SwiftUI.Axis?) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.TextField: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.TextField: SwiftUI.PrimitiveView {
}

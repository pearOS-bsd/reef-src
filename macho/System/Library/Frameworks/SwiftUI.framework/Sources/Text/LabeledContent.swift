// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LabeledContent (Text).

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

public struct LabeledContent<A, B> {
    let label: A
    let content: B
    init(label a0: A, content a1: B) { _skeleton() }
}

extension SwiftUI.LabeledContent where A: SwiftUI.View, B: SwiftUI.View {
    public init(content a0: () -> B, label a1: () -> A) { _skeleton() }
    func custom<A1>(_ a0: A1, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A1: SwiftUI.LabeledContentStyle { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.LabeledContent where A == SwiftUI.Text, B: SwiftUI.View {
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: Foundation.LocalizedStringResource, content a1: () -> B) { _skeleton() }
}

extension SwiftUI.LabeledContent where A == SwiftUI.Text, B == SwiftUI.Text {
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: A1) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: B1) where A1: Swift.StringProtocol, B1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: A1.FormatInput, format a2: A1) where A1: Foundation.FormatStyle, A1.FormatInput: Swift.Equatable, A1.FormatOutput == Swift.String { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: B1.FormatInput, format a2: B1) where A1: Swift.StringProtocol, B1: Foundation.FormatStyle, B1.FormatInput: Swift.Equatable, B1.FormatOutput == Swift.String { _skeleton() }
    public init<A1>(_ a0: Foundation.LocalizedStringResource, value a1: A1) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: Foundation.LocalizedStringResource, value a1: A1.FormatInput, format a2: A1) where A1: Foundation.FormatStyle, A1.FormatInput: Swift.Equatable, A1.FormatOutput == Swift.String { _skeleton() }
}

extension SwiftUI.LabeledContent where A == SwiftUI.LabeledContentStyleConfiguration.Label, B == SwiftUI.LabeledContentStyleConfiguration.Content {
    public init(_ a0: SwiftUI.LabeledContentStyleConfiguration) { _skeleton() }
}

extension SwiftUI.LabeledContent: SwiftUI.View where A: SwiftUI.View, B: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.LabeledContent: SwiftUI.PrimitiveView where A: SwiftUI.View, B: SwiftUI.View {
}

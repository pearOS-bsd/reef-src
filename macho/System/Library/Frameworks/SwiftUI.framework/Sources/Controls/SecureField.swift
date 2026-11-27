// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SecureField (Controls).

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

public struct SecureField<A> where A: SwiftUI.View {
    let label: A
    let text: SwiftUI.Binding<Swift.String>
    let prompt: SwiftUI.Text?
    let onCommit: () -> ()
    init(label a0: A, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?, onCommit a3: @escaping () -> ()) { _skeleton() }
    var field: SwiftUI.TextField<A> { _skeleton() }
    public init(text a0: SwiftUI.Binding<Swift.String>, prompt a1: SwiftUI.Text?, label a2: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.SecureField where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, prompt a2: SwiftUI.Text?) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, text a1: SwiftUI.Binding<Swift.String>, onCommit a2: @escaping () -> ()) { _skeleton() }
    public init<A1>(_ a0: A1, text a1: SwiftUI.Binding<Swift.String>, onCommit a2: @escaping () -> ()) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.SecureField: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.SecureField: SwiftUI.PrimitiveView {
}

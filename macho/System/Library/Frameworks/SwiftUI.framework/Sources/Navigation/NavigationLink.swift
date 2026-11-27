// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationLink (Navigation).

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

public struct NavigationLink<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let destination: B?
    let value: Swift.AnyHashable?
    let label: A
    let isActive: (SwiftUI.Binding<Swift.Bool>)?
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    init(destination a0: B?, value a1: Swift.AnyHashable?, label a2: A, isActive a3: (SwiftUI.Binding<Swift.Bool>)?) { _skeleton() }
    public init(destination a0: B, label a1: () -> A) { _skeleton() }
    public init(destination a0: () -> B, label a1: () -> A) { _skeleton() }
    public init(isActive a0: SwiftUI.Binding<Swift.Bool>, destination a1: () -> B, label a2: () -> A) { _skeleton() }
    public init(destination a0: B, isActive a1: SwiftUI.Binding<Swift.Bool>, label a2: () -> A) { _skeleton() }
}

extension SwiftUI.NavigationLink where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, destination a1: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, destination a1: () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, destination a1: B) { _skeleton() }
    public init<A1>(_ a0: A1, destination a1: B) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.NavigationLink where B == Swift.Never {
    public init<A1>(value a0: A1?, label a1: () -> A) where A1: Swift.Hashable { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: A1?) where A == SwiftUI.Text, A1: Swift.Hashable { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: B1?) where A == SwiftUI.Text, A1: Swift.StringProtocol, B1: Swift.Hashable { _skeleton() }
    public init<A1>(value a0: A1?, label a1: () -> A) where A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable { _skeleton() }
}

extension SwiftUI.NavigationLink: SwiftUI.PrimitiveView {
}

extension SwiftUI.NavigationLink: SwiftUI.View {
    public typealias Body = Swift.Never
}

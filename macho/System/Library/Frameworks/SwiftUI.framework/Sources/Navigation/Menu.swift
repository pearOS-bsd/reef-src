// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Menu (Navigation).

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

public struct Menu<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let label: A
    let content: B
    let primaryAction: (() -> ())?
    init(label a0: A, content a1: B, primaryAction a2: (() -> ())?) { _skeleton() }
    public init(content a0: () -> B, label a1: () -> A) { _skeleton() }
    public init(content a0: () -> B, label a1: () -> A, primaryAction a2: @escaping () -> ()) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B) where A == SwiftUI.Text { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> B) where A == SwiftUI.Text, A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B, primaryAction a2: @escaping () -> ()) where A == SwiftUI.Text { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> B, primaryAction a2: @escaping () -> ()) where A == SwiftUI.Text, A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, content a2: () -> B) where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, content a2: () -> B) where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image>, A1: Swift.StringProtocol { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}

extension SwiftUI.Menu where A == SwiftUI.MenuStyleConfiguration.Label, B == SwiftUI.MenuStyleConfiguration.Content {
    public init(_ a0: SwiftUI.MenuStyleConfiguration) { _skeleton() }
}

extension SwiftUI.Menu: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Menu: SwiftUI.PrimitiveView {
}

extension SwiftUI.Menu: SwiftUI.MenuContributor {
}

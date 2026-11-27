// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Tab (Navigation).

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

public struct Tab<A, B, C> {
    let value: A?
    let content: B
    let label: C
    let role: SwiftUI.TabRole?
    init(value a0: A?, content a1: B, label a2: C, role a3: SwiftUI.TabRole?) { _skeleton() }
}

extension SwiftUI.Tab where A: Swift.Hashable, B: SwiftUI.View, C: SwiftUI.View {
    public init(value a0: A, role a1: SwiftUI.TabRole?, content a2: () -> B) where C == SwiftUI.DefaultTabLabel { _skeleton() }
    public init(value a0: A, content a1: () -> B, label a2: () -> C) { _skeleton() }
    public init<A1>(_ a0: A1, image a1: Swift.String, value a2: A, content a3: () -> B) where C == SwiftUI.DefaultTabLabel, A1: Swift.StringProtocol { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    public var _identifiedView: some SwiftUI.TabContent { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
    public init(value a0: A, role a1: SwiftUI.TabRole?, content a2: () -> B, label a3: () -> C) { _skeleton() }
    public init<A1>(value a0: A1, role a1: SwiftUI.TabRole?, content a2: () -> B, label a3: () -> C) where A == A1?, A1: Swift.Hashable { _skeleton() }
}

extension SwiftUI.Tab where A == Swift.Never, B: SwiftUI.View, C: SwiftUI.View {
    public init(role a0: SwiftUI.TabRole?, content a1: () -> B, label a2: () -> C) { _skeleton() }
    public init(content a0: () -> B, label a1: () -> C) { _skeleton() }
}

extension SwiftUI.Tab where A: Swift.Hashable, B: SwiftUI.View, C == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, value a2: A, role a3: SwiftUI.TabRole?, content a4: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, value a2: A, role a3: SwiftUI.TabRole?, content a4: () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, image a1: Swift.String, value a2: A, role a3: SwiftUI.TabRole?, content a4: () -> B) { _skeleton() }
}

extension SwiftUI.Tab where A == Swift.Never, B: SwiftUI.View, C == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, role a2: SwiftUI.TabRole?, content a3: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, role a2: SwiftUI.TabRole?, content a3: () -> B) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Tab where A: Swift.Hashable, B: SwiftUI.View, C == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, value a1: A, role a2: SwiftUI.TabRole?, content a3: () -> B) { _skeleton() }
}

extension SwiftUI.Tab where A == Swift.Never, B: SwiftUI.View, C == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, role a1: SwiftUI.TabRole?, content a2: () -> B) { _skeleton() }
}

extension SwiftUI.Tab: SwiftUI.TabContent where A: Swift.Hashable, B: SwiftUI.View, C: SwiftUI.View {
    public typealias TabValue = A
    public typealias Body = Swift.Never
}

extension SwiftUI.Tab: SwiftUI.TabPagesProviding where A: Swift.Hashable, B: SwiftUI.View, C: SwiftUI.View {
}

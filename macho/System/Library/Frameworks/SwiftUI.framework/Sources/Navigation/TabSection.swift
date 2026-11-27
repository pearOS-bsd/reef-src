// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabSection (Navigation).

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

public struct TabSection<A, B, C, D> where A == C.TabValue, B: SwiftUI.View, C: SwiftUI.TabContent, D: SwiftUI.View {
    let header: B
    let content: C
    let footer: D
    public var body: Swift.Never { _skeleton() }
    init(header a0: B, content a1: C, footer a2: D) { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
}

extension SwiftUI.TabSection where D == SwiftUI.EmptyView {
    public init(content a0: () -> C, header a1: () -> B) { _skeleton() }
}

extension SwiftUI.TabSection where B == SwiftUI.Text, D == SwiftUI.EmptyView {
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> C) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> C) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.TabSection where B == SwiftUI.EmptyView, D == SwiftUI.EmptyView {
    public init(content a0: () -> C) { _skeleton() }
}

extension SwiftUI.TabSection: SwiftUI.TabContent {
    public typealias TabValue = A
    public typealias Body = Swift.Never
    public typealias _IdentifiedView = SwiftUI.TabSection<A, B, C, D>
}

extension SwiftUI.TabSection: SwiftUI.TabPagesProviding {
}

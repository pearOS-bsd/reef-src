// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyTabContent (Navigation).

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

public struct AnyTabContent<A> where A: Swift.Hashable {
    let pages: () -> [SwiftUI.TabPage]
    public init<A1>(_ a0: A1) where A == A1.TabValue, A1: SwiftUI.TabContent { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
}

extension SwiftUI.AnyTabContent: SwiftUI.TabContent {
    public typealias TabValue = A
    public typealias Body = Swift.Never
    public typealias _IdentifiedView = SwiftUI.AnyTabContent<A>
}

extension SwiftUI.AnyTabContent: SwiftUI.TabPagesProviding {
}

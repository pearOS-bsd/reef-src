// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TupleTabContent (Navigation).

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

public struct _TupleTabContent<A, B> where A: Swift.Hashable {
    public var value: B
    public init(_ a0: B) { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
}

extension SwiftUI._TupleTabContent: SwiftUI.TabContent {
    public typealias TabValue = A
    public typealias Body = Swift.Never
    public typealias _IdentifiedView = SwiftUI._TupleTabContent<A, B>
}

extension SwiftUI._TupleTabContent: SwiftUI.TabPagesProviding {
}

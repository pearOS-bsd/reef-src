// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OptionalTabContent (Navigation).

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

public struct _OptionalTabContent<A, B> where A: Swift.Hashable {
    let content: B?
    public var body: Swift.Never { _skeleton() }
    init(content a0: B?) { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
}

extension SwiftUI._OptionalTabContent: SwiftUI.TabContent {
    public typealias TabValue = A
    public typealias Body = Swift.Never
    public typealias _IdentifiedView = SwiftUI._OptionalTabContent<A, B>
}

extension SwiftUI._OptionalTabContent: SwiftUI.TabPagesProviding {
}

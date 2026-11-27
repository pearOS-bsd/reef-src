// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ConditionalTabContent (Navigation).

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

public struct _ConditionalTabContent<A, B, C> where A: Swift.Hashable {
    let storage: SwiftUI._ConditionalTabContent<A, B, C>.Storage
    public var body: Swift.Never { _skeleton() }
    init(storage a0: SwiftUI._ConditionalTabContent<A, B, C>.Storage) { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
    enum Storage {
        case trueContent(B)
        case falseContent(C)
    }
}

extension SwiftUI._ConditionalTabContent: SwiftUI.TabContent {
    public typealias TabValue = A
    public typealias Body = Swift.Never
    public typealias _IdentifiedView = SwiftUI._ConditionalTabContent<A, B, C>
}

extension SwiftUI._ConditionalTabContent: SwiftUI.TabPagesProviding {
}

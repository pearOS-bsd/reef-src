// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TupleTableColumnContent (Navigation).

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

public struct TupleTableColumnContent<A, B, C> where A: Swift.Identifiable, B: Foundation.SortComparator {
    public var value: C
    init(_ a0: C) { _skeleton() }
    public var tableColumnBody: Swift.Never { _skeleton() }
    func tableColumns() -> [SwiftUI.TableColumnEntry] { _skeleton() }
}

extension SwiftUI.TupleTableColumnContent: SwiftUI.TableColumnContent {
    public typealias TableRowValue = A
    public typealias TableColumnSortComparator = B
    public typealias TableColumnBody = Swift.Never
}

extension SwiftUI.TupleTableColumnContent: SwiftUI.TableColumnsSource {
}

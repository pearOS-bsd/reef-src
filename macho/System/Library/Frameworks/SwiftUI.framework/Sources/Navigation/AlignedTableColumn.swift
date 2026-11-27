// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AlignedTableColumn (Navigation).

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

struct AlignedTableColumn<A> where A: SwiftUI.TableColumnContent {
    let base: A
    let alignment: SwiftUI.TableColumnAlignment
    func tableColumns() -> [SwiftUI.TableColumnEntry] { _skeleton() }
    init(base a0: A, alignment a1: SwiftUI.TableColumnAlignment) { _skeleton() }
}

extension SwiftUI.AlignedTableColumn: SwiftUI.TableColumnsSource {
}

extension SwiftUI.AlignedTableColumn: SwiftUI.TableColumnContent {
    typealias TableRowValue = A.TableRowValue
    typealias TableColumnSortComparator = A.TableColumnSortComparator
    typealias TableColumnBody = Swift.Never
    var tableColumnBody: Self.TableColumnBody { _skeleton() }
}

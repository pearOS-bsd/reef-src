// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableColumnContent (Navigation).

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

public protocol TableColumnContent {
    associatedtype TableRowValue: Swift.Identifiable
    associatedtype TableColumnSortComparator: Foundation.SortComparator
    associatedtype TableColumnBody: SwiftUI.TableColumnContent
    var tableColumnBody: Self.TableColumnBody { get }
}

extension SwiftUI.TableColumnContent {
    public func alignment(_ a0: SwiftUI.TableColumnAlignment) -> some SwiftUI.TableColumnContent { self }
}

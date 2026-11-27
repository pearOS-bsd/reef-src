// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableColumnBuilder (Navigation).

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

public struct TableColumnBuilder<A, B> where A: Swift.Identifiable, B: Foundation.SortComparator {
    public static func buildExpression<A1>(_ a0: A1) -> A1 where A == A1.TableRowValue, A1: SwiftUI.TableColumnContent { _skeleton() }
    public static func buildBlock<A1>(_ a0: A1) -> A1 where A == A1.TableRowValue, B == A1.TableColumnSortComparator, A1: SwiftUI.TableColumnContent { _skeleton() }
    public static func buildBlock<A1, B1>(_ a0: A1, _ a1: B1) -> SwiftUI.TupleTableColumnContent<A, B, (A1, B1)> where A1: SwiftUI.TableColumnContent, B1: SwiftUI.TableColumnContent { _skeleton() }
    public static func buildBlock<A1, B1, C1>(_ a0: A1, _ a1: B1, _ a2: C1) -> SwiftUI.TupleTableColumnContent<A, B, (A1, B1, C1)> where A1: SwiftUI.TableColumnContent, B1: SwiftUI.TableColumnContent, C1: SwiftUI.TableColumnContent { _skeleton() }
    public static func buildBlock<A1, B1, C1, D1>(_ a0: A1, _ a1: B1, _ a2: C1, _ a3: D1) -> SwiftUI.TupleTableColumnContent<A, B, (A1, B1, C1, D1)> where A1: SwiftUI.TableColumnContent, B1: SwiftUI.TableColumnContent, C1: SwiftUI.TableColumnContent, D1: SwiftUI.TableColumnContent { _skeleton() }
    public static func buildBlock<A1, B1, C1, D1, E1>(_ a0: A1, _ a1: B1, _ a2: C1, _ a3: D1, _ a4: E1) -> SwiftUI.TupleTableColumnContent<A, B, (A1, B1, C1, D1, E1)> where A1: SwiftUI.TableColumnContent, B1: SwiftUI.TableColumnContent, C1: SwiftUI.TableColumnContent, D1: SwiftUI.TableColumnContent, E1: SwiftUI.TableColumnContent { _skeleton() }
    public static func buildBlock<A1, B1, C1, D1, E1, F1>(_ a0: A1, _ a1: B1, _ a2: C1, _ a3: D1, _ a4: E1, _ a5: F1) -> SwiftUI.TupleTableColumnContent<A, B, (A1, B1, C1, D1, E1, F1)> where A1: SwiftUI.TableColumnContent, B1: SwiftUI.TableColumnContent, C1: SwiftUI.TableColumnContent, D1: SwiftUI.TableColumnContent, E1: SwiftUI.TableColumnContent, F1: SwiftUI.TableColumnContent { _skeleton() }
}

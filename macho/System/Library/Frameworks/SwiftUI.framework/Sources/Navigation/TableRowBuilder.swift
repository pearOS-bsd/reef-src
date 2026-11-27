// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableRowBuilder (Navigation).

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

public struct TableRowBuilder<A> where A: Swift.Identifiable {
    public static func buildBlock() -> SwiftUI.TupleTableRowContent<A, ()> { _skeleton() }
    public static func buildBlock<A1>(_ a0: A1) -> A1 where A == A1.TableRowValue, A1: SwiftUI.TableRowContent { _skeleton() }
    public static func buildBlock<A1, B1>(_ a0: A1, _ a1: B1) -> SwiftUI.TupleTableRowContent<A, (A1, B1)> where A == A1.TableRowValue, A1: SwiftUI.TableRowContent, B1: SwiftUI.TableRowContent, A1.TableRowValue == B1.TableRowValue { _skeleton() }
    public static func buildBlock<A1, B1, C1>(_ a0: A1, _ a1: B1, _ a2: C1) -> SwiftUI.TupleTableRowContent<A, (A1, B1, C1)> where A == A1.TableRowValue, A1: SwiftUI.TableRowContent, B1: SwiftUI.TableRowContent, C1: SwiftUI.TableRowContent, A1.TableRowValue == B1.TableRowValue, B1.TableRowValue == C1.TableRowValue { _skeleton() }
    public static func buildBlock<A1, B1, C1, D1>(_ a0: A1, _ a1: B1, _ a2: C1, _ a3: D1) -> SwiftUI.TupleTableRowContent<A, (A1, B1, C1, D1)> where A == A1.TableRowValue, A1: SwiftUI.TableRowContent, B1: SwiftUI.TableRowContent, C1: SwiftUI.TableRowContent, D1: SwiftUI.TableRowContent, A1.TableRowValue == B1.TableRowValue, B1.TableRowValue == C1.TableRowValue, C1.TableRowValue == D1.TableRowValue { _skeleton() }
    public static func buildBlock<A1, B1, C1, D1, E1>(_ a0: A1, _ a1: B1, _ a2: C1, _ a3: D1, _ a4: E1) -> SwiftUI.TupleTableRowContent<A, (A1, B1, C1, D1, E1)> where A == A1.TableRowValue, A1: SwiftUI.TableRowContent, B1: SwiftUI.TableRowContent, C1: SwiftUI.TableRowContent, D1: SwiftUI.TableRowContent, E1: SwiftUI.TableRowContent, A1.TableRowValue == B1.TableRowValue, B1.TableRowValue == C1.TableRowValue, C1.TableRowValue == D1.TableRowValue, D1.TableRowValue == E1.TableRowValue { _skeleton() }
    public static func buildIf<A1>(_ a0: A1?) -> A1? where A1: SwiftUI.TableRowContent { _skeleton() }
    public static func buildEither<A1, B1>(first a0: A1) -> SwiftUI._ConditionalContent<A1, B1> where A1: SwiftUI.TableRowContent, B1: SwiftUI.TableRowContent { _skeleton() }
    public static func buildEither<A1, B1>(second a0: B1) -> SwiftUI._ConditionalContent<A1, B1> where A1: SwiftUI.TableRowContent, B1: SwiftUI.TableRowContent { _skeleton() }
    public static func buildExpression<A1>(_ a0: A1) -> A1 where A == A1.TableRowValue, A1: SwiftUI.TableRowContent { _skeleton() }
}

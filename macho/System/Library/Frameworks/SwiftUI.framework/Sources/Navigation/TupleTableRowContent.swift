// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TupleTableRowContent (Navigation).

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

public struct TupleTableRowContent<A, B> where A: Swift.Identifiable {
    public var value: B
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
    public init(_ a0: B) { _skeleton() }
}

extension SwiftUI.TupleTableRowContent: SwiftUI.TableRowContent {
    public typealias TableRowValue = A
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.TupleTableRowContent: SwiftUI.TableRowsSource {
}

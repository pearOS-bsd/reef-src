// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableRow (Navigation).

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

public struct TableRow<A> where A: Swift.Identifiable {
    let value: A
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}

extension SwiftUI.TableRow: SwiftUI.TableRowContent {
    public typealias TableRowValue = A
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.TableRow: SwiftUI.TableRowsSource {
}

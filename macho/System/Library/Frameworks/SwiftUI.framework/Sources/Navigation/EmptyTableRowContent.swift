// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EmptyTableRowContent (Navigation).

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

public struct EmptyTableRowContent<A> where A: Swift.Identifiable {
    public var tableRowBody: Swift.Never { _skeleton() }
    init() { _skeleton() }
}

extension SwiftUI.EmptyTableRowContent: SwiftUI.TableRowContent {
    public typealias TableRowValue = A
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.EmptyTableRowContent: SwiftUI.TableRowsSource {
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}

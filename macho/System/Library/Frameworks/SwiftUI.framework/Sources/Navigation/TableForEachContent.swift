// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableForEachContent (Navigation).

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

public struct TableForEachContent<A> where A: Swift.RandomAccessCollection, A.Element: Swift.Identifiable {
    public var data: A
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
    init(data a0: A) { _skeleton() }
}

extension SwiftUI.TableForEachContent: SwiftUI.DynamicTableRowContent {
    public typealias Data = A
}

extension SwiftUI.TableForEachContent: SwiftUI.TableRowsSource {
}

extension SwiftUI.TableForEachContent: SwiftUI.TableRowContent {
    public typealias TableRowValue = A.Element
    public typealias TableRowBody = Swift.Never
}

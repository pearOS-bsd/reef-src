// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableRowContent (Navigation).

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

public protocol TableRowContent {
    associatedtype TableRowValue: Swift.Identifiable
    associatedtype TableRowBody: SwiftUI.TableRowContent
    var tableRowBody: Self.TableRowBody { get }
}

extension SwiftUI.TableRowContent {
    public func draggable<A1>(_ a0: @escaping @autoclosure () -> A1) -> some SwiftUI.TableRowContent where A1: CoreTransferable.Transferable { self }
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DynamicTableRowContent (Navigation).

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

public protocol DynamicTableRowContent: SwiftUI.TableRowContent {
    associatedtype Data: Swift.Collection
    var data: Self.Data { get }
}

extension SwiftUI.DynamicTableRowContent {
    public func dropDestination<A1>(`for` a0: A1.Type, action a1: @escaping (Swift.Int, [A1]) -> ()) -> SwiftUI.ModifiedContent<Self, SwiftUI.OnInsertTableRowModifier> where A1: CoreTransferable.Transferable { _skeleton() }
}

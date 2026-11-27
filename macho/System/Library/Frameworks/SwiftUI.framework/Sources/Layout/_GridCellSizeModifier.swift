// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _GridCellSizeModifier (Layout).

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

public struct _GridCellSizeModifier {
    public var columnCount: Swift.Int
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(columnCount a0: Swift.Int) { _skeleton() }
}

extension SwiftUI._GridCellSizeModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._GridCellSizeModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

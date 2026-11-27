// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SymbolRenderingMode (Images).

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

public struct SymbolRenderingMode {
    let rawValue: Swift.Int
    public static var monochrome: SwiftUI.SymbolRenderingMode { _skeleton() }
    public static var multicolor: SwiftUI.SymbolRenderingMode { _skeleton() }
    public static var hierarchical: SwiftUI.SymbolRenderingMode { _skeleton() }
    public static var palette: SwiftUI.SymbolRenderingMode { _skeleton() }
    public static func == (_ a0: SwiftUI.SymbolRenderingMode, _ a1: SwiftUI.SymbolRenderingMode) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.SymbolRenderingMode: Swift.Equatable {
}

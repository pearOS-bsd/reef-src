// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _GridAnchorModifier (Layout).

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

public struct _GridAnchorModifier {
    public var anchor: SwiftUI.UnitPoint
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(anchor a0: SwiftUI.UnitPoint) { _skeleton() }
}

extension SwiftUI._GridAnchorModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._GridAnchorModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

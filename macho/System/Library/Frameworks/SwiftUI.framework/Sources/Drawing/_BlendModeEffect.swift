// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _BlendModeEffect (Drawing).

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

public struct _BlendModeEffect {
    public var blendMode: SwiftUI.BlendMode
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(blendMode a0: SwiftUI.BlendMode) { _skeleton() }
}

extension SwiftUI._BlendModeEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._BlendModeEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

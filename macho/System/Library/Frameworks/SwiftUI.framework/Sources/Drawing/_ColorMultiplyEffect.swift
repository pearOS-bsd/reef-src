// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ColorMultiplyEffect (Drawing).

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

public struct _ColorMultiplyEffect {
    public var color: SwiftUI.Color
    public init(color a0: SwiftUI.Color) { _skeleton() }
}

extension SwiftUI._ColorMultiplyEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._ColorMultiplyEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

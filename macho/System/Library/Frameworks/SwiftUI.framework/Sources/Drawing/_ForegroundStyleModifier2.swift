// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ForegroundStyleModifier2 (Drawing).

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

public struct _ForegroundStyleModifier2<A, B> where A: SwiftUI.ShapeStyle, B: SwiftUI.ShapeStyle {
    public var primary: A
    public var secondary: B
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(primary a0: A, secondary a1: B) { _skeleton() }
}

extension SwiftUI._ForegroundStyleModifier2: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._ForegroundStyleModifier2: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

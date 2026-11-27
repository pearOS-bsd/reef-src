// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ForegroundStyleModifier3 (Drawing).

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

public struct _ForegroundStyleModifier3<A, B, C> where A: SwiftUI.ShapeStyle, B: SwiftUI.ShapeStyle, C: SwiftUI.ShapeStyle {
    public var primary: A
    public var secondary: B
    public var tertiary: C
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(primary a0: A, secondary a1: B, tertiary a2: C) { _skeleton() }
}

extension SwiftUI._ForegroundStyleModifier3: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._ForegroundStyleModifier3: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

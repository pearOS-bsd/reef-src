// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ForegroundStyleModifier (Drawing).

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

public struct _ForegroundStyleModifier<A> where A: SwiftUI.ShapeStyle {
    public var style: A
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(style a0: A) { _skeleton() }
}

extension SwiftUI._ForegroundStyleModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._ForegroundStyleModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

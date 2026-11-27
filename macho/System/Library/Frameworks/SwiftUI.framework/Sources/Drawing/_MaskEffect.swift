// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _MaskEffect (Drawing).

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

public struct _MaskEffect<A> where A: SwiftUI.View {
    public var mask: A
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(mask a0: A) { _skeleton() }
}

extension SwiftUI._MaskEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._MaskEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

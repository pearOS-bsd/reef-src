// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OverlayStyleModifier (Layout).

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

public struct _OverlayStyleModifier<A> where A: SwiftUI.ShapeStyle {
    public var style: A
    public var ignoresSafeAreaEdges: SwiftUI.Edge.Set
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(style a0: A, ignoresSafeAreaEdges a1: SwiftUI.Edge.Set) { _skeleton() }
}

extension SwiftUI._OverlayStyleModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._OverlayStyleModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

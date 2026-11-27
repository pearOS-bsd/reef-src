// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _PaddingLayout (Layout).

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

public struct _PaddingLayout {
    public var edges: SwiftUI.Edge.Set
    public var insets: SwiftUI.EdgeInsets?
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(edges a0: SwiftUI.Edge.Set, insets a1: SwiftUI.EdgeInsets?) { _skeleton() }
}

extension SwiftUI._PaddingLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._PaddingLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._PaddingLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

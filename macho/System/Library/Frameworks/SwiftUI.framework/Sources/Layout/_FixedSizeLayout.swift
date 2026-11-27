// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _FixedSizeLayout (Layout).

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

public struct _FixedSizeLayout {
    public var horizontal: Swift.Bool
    public var vertical: Swift.Bool
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(horizontal a0: Swift.Bool, vertical a1: Swift.Bool) { _skeleton() }
}

extension SwiftUI._FixedSizeLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._FixedSizeLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._FixedSizeLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

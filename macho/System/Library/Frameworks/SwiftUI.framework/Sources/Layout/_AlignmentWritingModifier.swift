// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _AlignmentWritingModifier (Layout).

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

public struct _AlignmentWritingModifier {
    public let key: SwiftUI.AlignmentKey
    public let computeValue: (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(key a0: SwiftUI.AlignmentKey, computeValue a1: @escaping (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat) { _skeleton() }
}

extension SwiftUI._AlignmentWritingModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._AlignmentWritingModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScaleTransition (Animation).

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

public struct ScaleTransition {
    public var scale: Swift.Double
    public var anchor: SwiftUI.UnitPoint
    public func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.ScaleTransition>, phase a1: SwiftUI.TransitionPhase) -> SwiftUI.PlaceholderContentView<SwiftUI.ScaleTransition> { _skeleton() }
    public init(_ a0: Swift.Double, anchor a1: SwiftUI.UnitPoint) { _skeleton() }
}

extension SwiftUI.ScaleTransition: SwiftUI.Transition {
    public typealias Body = SwiftUI.PlaceholderContentView<SwiftUI.ScaleTransition>
}

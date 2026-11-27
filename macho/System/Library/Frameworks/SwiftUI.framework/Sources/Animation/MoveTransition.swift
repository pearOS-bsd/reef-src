// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MoveTransition (Animation).

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

public struct MoveTransition {
    public var edge: SwiftUI.Edge
    public func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.MoveTransition>, phase a1: SwiftUI.TransitionPhase) -> SwiftUI.PlaceholderContentView<SwiftUI.MoveTransition> { _skeleton() }
    public init(edge a0: SwiftUI.Edge) { _skeleton() }
}

extension SwiftUI.MoveTransition: SwiftUI.Transition {
    public typealias Body = SwiftUI.PlaceholderContentView<SwiftUI.MoveTransition>
}

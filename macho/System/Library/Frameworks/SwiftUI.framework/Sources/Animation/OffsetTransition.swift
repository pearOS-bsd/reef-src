// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OffsetTransition (Animation).

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

public struct OffsetTransition {
    public var offset: CGSize
    public init(_ a0: CGSize) { _skeleton() }
    public func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.OffsetTransition>, phase a1: SwiftUI.TransitionPhase) -> SwiftUI.PlaceholderContentView<SwiftUI.OffsetTransition> { _skeleton() }
}

extension SwiftUI.OffsetTransition: SwiftUI.Transition {
    public typealias Body = SwiftUI.PlaceholderContentView<SwiftUI.OffsetTransition>
}

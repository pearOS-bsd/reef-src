// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BlurReplaceTransition (Animation).

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

public struct BlurReplaceTransition {
    public var configuration: SwiftUI.BlurReplaceTransition.Configuration
    public init(configuration a0: SwiftUI.BlurReplaceTransition.Configuration) { _skeleton() }
    public func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.BlurReplaceTransition>, phase a1: SwiftUI.TransitionPhase) -> SwiftUI.PlaceholderContentView<SwiftUI.BlurReplaceTransition> { _skeleton() }
    public struct Configuration {
        let rawValue: Swift.Int
        public static var downUp: SwiftUI.BlurReplaceTransition.Configuration { SwiftUI.BlurReplaceTransition.Configuration(rawValue: 0) }
        public static var upUp: SwiftUI.BlurReplaceTransition.Configuration { SwiftUI.BlurReplaceTransition.Configuration(rawValue: 1) }
        public static func == (_ a0: SwiftUI.BlurReplaceTransition.Configuration, _ a1: SwiftUI.BlurReplaceTransition.Configuration) -> Swift.Bool { a0.rawValue == a1.rawValue }
    }
}

extension SwiftUI.BlurReplaceTransition.Configuration: Swift.Equatable {
}

extension SwiftUI.BlurReplaceTransition: SwiftUI.Transition {
    public typealias Body = SwiftUI.PlaceholderContentView<SwiftUI.BlurReplaceTransition>
}

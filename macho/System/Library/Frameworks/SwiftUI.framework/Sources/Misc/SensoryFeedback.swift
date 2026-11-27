// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SensoryFeedback (Misc).

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

public struct SensoryFeedback {
    let kind: Swift.Int
    public static var alignment: SwiftUI.SensoryFeedback { _skeleton() }
    public static var levelChange: SwiftUI.SensoryFeedback { _skeleton() }
    public static func impact(flexibility a0: SwiftUI.SensoryFeedback.Flexibility, intensity a1: Swift.Double) -> SwiftUI.SensoryFeedback { _skeleton() }
    public static var success: SwiftUI.SensoryFeedback { _skeleton() }
    public static var warning: SwiftUI.SensoryFeedback { _skeleton() }
    public static var error: SwiftUI.SensoryFeedback { _skeleton() }
    public static var selection: SwiftUI.SensoryFeedback { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.SensoryFeedback, _ a1: SwiftUI.SensoryFeedback) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public struct Flexibility {
        let rawValue: Swift.Int
        public static var rigid: SwiftUI.SensoryFeedback.Flexibility { _skeleton() }
        public static var solid: SwiftUI.SensoryFeedback.Flexibility { _skeleton() }
        public static var soft: SwiftUI.SensoryFeedback.Flexibility { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public static func == (_ a0: SwiftUI.SensoryFeedback.Flexibility, _ a1: SwiftUI.SensoryFeedback.Flexibility) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    }
}

extension SwiftUI.SensoryFeedback.Flexibility: Swift.Hashable {
}

extension SwiftUI.SensoryFeedback.Flexibility: Swift.Equatable {
}

extension SwiftUI.SensoryFeedback: Swift.Hashable {
}

extension SwiftUI.SensoryFeedback: Swift.Equatable {
}

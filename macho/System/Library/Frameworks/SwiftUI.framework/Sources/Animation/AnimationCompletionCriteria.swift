// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationCompletionCriteria (Animation).

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

public struct AnimationCompletionCriteria {
    let rawValue: Swift.Int
    public static var logicallyComplete: SwiftUI.AnimationCompletionCriteria { SwiftUI.AnimationCompletionCriteria(rawValue: 0) }
    public static var removed: SwiftUI.AnimationCompletionCriteria { SwiftUI.AnimationCompletionCriteria(rawValue: 1) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.AnimationCompletionCriteria, _ a1: SwiftUI.AnimationCompletionCriteria) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.AnimationCompletionCriteria: Swift.Hashable {
}

extension SwiftUI.AnimationCompletionCriteria: Swift.Equatable {
}

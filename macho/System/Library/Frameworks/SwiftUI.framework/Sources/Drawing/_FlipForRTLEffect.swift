// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _FlipForRTLEffect (Drawing).

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

public struct _FlipForRTLEffect {
    public var isEnabled: Swift.Bool
    public init(isEnabled a0: Swift.Bool) { _skeleton() }
    public static func == (_ a0: SwiftUI._FlipForRTLEffect, _ a1: SwiftUI._FlipForRTLEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._FlipForRTLEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._FlipForRTLEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._FlipForRTLEffect: Swift.Equatable {
}

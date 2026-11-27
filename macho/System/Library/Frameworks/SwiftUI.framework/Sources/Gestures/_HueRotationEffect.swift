// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _HueRotationEffect (Gestures).

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

public struct _HueRotationEffect {
    public var angle: SwiftUI.Angle
    public init(angle a0: SwiftUI.Angle) { _skeleton() }
    public static func == (_ a0: SwiftUI._HueRotationEffect, _ a1: SwiftUI._HueRotationEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._HueRotationEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._HueRotationEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._HueRotationEffect: Swift.Equatable {
}

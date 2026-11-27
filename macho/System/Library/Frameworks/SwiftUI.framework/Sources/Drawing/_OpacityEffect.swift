// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OpacityEffect (Drawing).

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

public struct _OpacityEffect {
    public var opacity: Swift.Double
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(opacity a0: Swift.Double) { _skeleton() }
    public static func == (_ a0: SwiftUI._OpacityEffect, _ a1: SwiftUI._OpacityEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._OpacityEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._OpacityEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._OpacityEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._OpacityEffect: Swift.Equatable {
}

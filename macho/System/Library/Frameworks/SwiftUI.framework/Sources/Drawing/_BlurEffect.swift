// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _BlurEffect (Drawing).

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

public struct _BlurEffect {
    public var radius: CoreGraphics.CGFloat
    public var isOpaque: Swift.Bool
    public init(radius a0: CoreGraphics.CGFloat, opaque a1: Swift.Bool) { _skeleton() }
    public static func == (_ a0: SwiftUI._BlurEffect, _ a1: SwiftUI._BlurEffect) -> Swift.Bool { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._BlurEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._BlurEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._BlurEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._BlurEffect: Swift.Equatable {
}

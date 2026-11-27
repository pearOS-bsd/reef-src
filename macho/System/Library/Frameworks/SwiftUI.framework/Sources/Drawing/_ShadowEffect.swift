// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ShadowEffect (Drawing).

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

public struct _ShadowEffect {
    public var color: SwiftUI.Color
    public var radius: CoreGraphics.CGFloat
    public var offset: CGSize
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(color a0: SwiftUI.Color, radius a1: CoreGraphics.CGFloat, offset a2: CGSize) { _skeleton() }
    public static func == (_ a0: SwiftUI._ShadowEffect, _ a1: SwiftUI._ShadowEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._ShadowEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._ShadowEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._ShadowEffect: Swift.Equatable {
}

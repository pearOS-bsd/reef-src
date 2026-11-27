// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _DrawingGroupEffect (Drawing).

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

public struct _DrawingGroupEffect {
    public var isOpaque: Swift.Bool
    public var colorMode: SwiftUI.ColorRenderingMode
    public init(opaque a0: Swift.Bool, colorMode a1: SwiftUI.ColorRenderingMode) { _skeleton() }
    public static func == (_ a0: SwiftUI._DrawingGroupEffect, _ a1: SwiftUI._DrawingGroupEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._DrawingGroupEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._DrawingGroupEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._DrawingGroupEffect: Swift.Equatable {
}

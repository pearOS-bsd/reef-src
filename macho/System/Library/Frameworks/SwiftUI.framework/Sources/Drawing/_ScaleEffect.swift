// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ScaleEffect (Drawing).

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

public struct _ScaleEffect {
    public var scale: CGSize
    public var anchor: SwiftUI.UnitPoint
    public static func == (_ a0: SwiftUI._ScaleEffect, _ a1: SwiftUI._ScaleEffect) -> Swift.Bool { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(scale a0: CGSize, anchor a1: SwiftUI.UnitPoint) { _skeleton() }
}

extension SwiftUI._ScaleEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._ScaleEffect: Swift.Equatable {
}

extension SwiftUI._ScaleEffect: SwiftUI.PrimitiveModifier {
}

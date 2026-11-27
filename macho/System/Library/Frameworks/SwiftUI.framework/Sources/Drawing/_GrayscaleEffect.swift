// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _GrayscaleEffect (Drawing).

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

public struct _GrayscaleEffect {
    public var amount: Swift.Double
    public init(amount a0: Swift.Double) { _skeleton() }
    public static func == (_ a0: SwiftUI._GrayscaleEffect, _ a1: SwiftUI._GrayscaleEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._GrayscaleEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._GrayscaleEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._GrayscaleEffect: Swift.Equatable {
}

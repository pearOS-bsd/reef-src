// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TransformEffect (Drawing).

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

public struct _TransformEffect {
    public var transform: CGAffineTransform
    public init(transform a0: CGAffineTransform) { _skeleton() }
    public static func == (_ a0: SwiftUI._TransformEffect, _ a1: SwiftUI._TransformEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._TransformEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._TransformEffect: Swift.Equatable {
}

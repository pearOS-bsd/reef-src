// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to Swift_Float.

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

extension Swift.Float {
    public var animatableData: Swift.Float {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var _arg: Swift.Float { self }
    public var _specifier: Swift.String { "%f" }
    public func scale(by a0: Swift.Double) { _skeleton() }
    public var magnitudeSquared: Swift.Double { _skeleton() }
}

extension Swift.Float: SwiftUI.Animatable {
    public typealias AnimatableData = Swift.Float
}

extension Swift.Float: SwiftUI._FormatSpecifiable {
    public typealias _Arg = Swift.Float
}

extension Swift.Float: SwiftUI.VectorArithmetic {
}

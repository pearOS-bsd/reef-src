// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to Swift_Double.

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

extension Swift.Double {
    public var animatableData: Swift.Double {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var _arg: Swift.Double { self }
    public var _specifier: Swift.String { "%lf" }
    public func scale(by a0: Swift.Double) { _skeleton() }
    public var magnitudeSquared: Swift.Double { _skeleton() }
}

extension Swift.Double: SwiftUI.Animatable {
    public typealias AnimatableData = Swift.Double
}

extension Swift.Double: SwiftUI._FormatSpecifiable {
    public typealias _Arg = Swift.Double
}

extension Swift.Double: SwiftUI.VectorArithmetic {
}

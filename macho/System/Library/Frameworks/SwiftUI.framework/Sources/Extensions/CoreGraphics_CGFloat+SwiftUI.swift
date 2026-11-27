// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to CoreGraphics_CGFloat.

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

extension CoreGraphics.CGFloat {
    public var animatableData: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var _arg: CoreGraphics.CGFloat { _skeleton() }
    public var _specifier: Swift.String { _skeleton() }
    public func scale(by a0: Swift.Double) { _skeleton() }
    public var magnitudeSquared: Swift.Double { _skeleton() }
}

extension CoreGraphics.CGFloat: SwiftUI.Animatable {
    public typealias AnimatableData = CoreGraphics.CGFloat
}

extension CoreGraphics.CGFloat: SwiftUI._FormatSpecifiable {
    public typealias _Arg = CoreGraphics.CGFloat
}

extension CoreGraphics.CGFloat: SwiftUI.VectorArithmetic {
}

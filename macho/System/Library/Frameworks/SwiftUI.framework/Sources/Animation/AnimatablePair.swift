// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimatablePair (Animation).

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

public struct AnimatablePair<A, B> where A: SwiftUI.VectorArithmetic, B: SwiftUI.VectorArithmetic {
    public var first: A
    public var second: B
    public init(_ a0: A, _ a1: B) { _skeleton() }
    public static var zero: SwiftUI.AnimatablePair<A, B> { _skeleton() }
    public static func + (_ a0: SwiftUI.AnimatablePair<A, B>, _ a1: SwiftUI.AnimatablePair<A, B>) -> SwiftUI.AnimatablePair<A, B> { _skeleton() }
    public static func - (_ a0: SwiftUI.AnimatablePair<A, B>, _ a1: SwiftUI.AnimatablePair<A, B>) -> SwiftUI.AnimatablePair<A, B> { _skeleton() }
    public func scale(by a0: Swift.Double) { _skeleton() }
    public var magnitudeSquared: Swift.Double { _skeleton() }
    public static func == (_ a0: SwiftUI.AnimatablePair<A, B>, _ a1: SwiftUI.AnimatablePair<A, B>) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.AnimatablePair: SwiftUI.VectorArithmetic {
}

extension SwiftUI.AnimatablePair: Swift.AdditiveArithmetic {
}

extension SwiftUI.AnimatablePair: Swift.Equatable {
}

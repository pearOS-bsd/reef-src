// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Spring (Animation).

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

public struct Spring {
    public var duration: Swift.Double
    public var bounce: Swift.Double
    var mass: Swift.Double
    public init(response a0: Swift.Double, dampingRatio a1: Swift.Double) { _skeleton() }
    public init(mass a0: Swift.Double, stiffness a1: Swift.Double, damping a2: Swift.Double, allowOverDamping a3: Swift.Bool) { _skeleton() }
    public init(settlingDuration a0: Swift.Double, dampingRatio a1: Swift.Double, epsilon a2: Swift.Double) { _skeleton() }
    public var dampingRatio: Swift.Double { _skeleton() }
    public var stiffness: Swift.Double { _skeleton() }
    public var damping: Swift.Double { _skeleton() }
    public var settlingDuration: Swift.Double { _skeleton() }
    public static var smooth: SwiftUI.Spring { _skeleton() }
    public static func smooth(duration a0: Swift.Double, extraBounce a1: Swift.Double) -> SwiftUI.Spring { _skeleton() }
    public static var snappy: SwiftUI.Spring { _skeleton() }
    public static func snappy(duration a0: Swift.Double, extraBounce a1: Swift.Double) -> SwiftUI.Spring { _skeleton() }
    public static var bouncy: SwiftUI.Spring { _skeleton() }
    public static func bouncy(duration a0: Swift.Double, extraBounce a1: Swift.Double) -> SwiftUI.Spring { _skeleton() }
    public func value<A>(target a0: A, initialVelocity a1: A, time a2: Swift.Double) -> A where A: SwiftUI.VectorArithmetic { _skeleton() }
    public func velocity<A>(target a0: A, initialVelocity a1: A, time a2: Swift.Double) -> A where A: SwiftUI.VectorArithmetic { _skeleton() }
    public static func == (_ a0: SwiftUI.Spring, _ a1: SwiftUI.Spring) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public var response: Swift.Double { _skeleton() }
    public init(duration a0: Swift.Double, bounce a1: Swift.Double) { _skeleton() }
}

extension SwiftUI.Spring: Swift.Hashable {
}

extension SwiftUI.Spring: Swift.Equatable {
}

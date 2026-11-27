// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Animation (Animation).

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

public struct Animation {
    public var box: SwiftUI.AnimationBoxBase
    public init<A>(_ a0: A) where A: SwiftUI.CustomAnimation { _skeleton() }
    public func animate<A>(value a0: A, time a1: Swift.Double, context a2: inout SwiftUI.AnimationContext<A>) -> A? where A: SwiftUI.VectorArithmetic { _skeleton() }
    public func velocity<A>(value a0: A, time a1: Swift.Double, context a2: SwiftUI.AnimationContext<A>) -> A? where A: SwiftUI.VectorArithmetic { _skeleton() }
    public func shouldMerge<A>(previous a0: SwiftUI.Animation, value a1: A, time a2: Swift.Double, context a3: inout SwiftUI.AnimationContext<A>) -> Swift.Bool where A: SwiftUI.VectorArithmetic { _skeleton() }
    public var base: any SwiftUI.CustomAnimation { _skeleton() }
    public var description: Swift.String { _skeleton() }
    public var debugDescription: Swift.String { _skeleton() }
    public static var `default`: SwiftUI.Animation { _skeleton() }
    public static func linear(duration a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func timingCurve(_ a0: SwiftUI.UnitCurve, duration a1: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var linear: SwiftUI.Animation { _skeleton() }
    public static func easeIn(duration a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var easeIn: SwiftUI.Animation { _skeleton() }
    public static func easeOut(duration a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var easeOut: SwiftUI.Animation { _skeleton() }
    public static func easeInOut(duration a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var easeInOut: SwiftUI.Animation { _skeleton() }
    public static func timingCurve(_ a0: Swift.Double, _ a1: Swift.Double, _ a2: Swift.Double, _ a3: Swift.Double, duration a4: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func spring(duration a0: Swift.Double, bounce a1: Swift.Double, blendDuration a2: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func spring(_ a0: SwiftUI.Spring, blendDuration a1: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func interactiveSpring(response a0: Swift.Double, dampingFraction a1: Swift.Double, blendDuration a2: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func interactiveSpring(duration a0: Swift.Double, extraBounce a1: Swift.Double, blendDuration a2: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var interactiveSpring: SwiftUI.Animation { _skeleton() }
    public static func interpolatingSpring(mass a0: Swift.Double, stiffness a1: Swift.Double, damping a2: Swift.Double, initialVelocity a3: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func interpolatingSpring(duration a0: Swift.Double, bounce a1: Swift.Double, initialVelocity a2: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static func interpolatingSpring(_ a0: SwiftUI.Spring, initialVelocity a1: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var interpolatingSpring: SwiftUI.Animation { _skeleton() }
    public static var smooth: SwiftUI.Animation { _skeleton() }
    public static func smooth(duration a0: Swift.Double, extraBounce a1: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var snappy: SwiftUI.Animation { _skeleton() }
    public static func snappy(duration a0: Swift.Double, extraBounce a1: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var bouncy: SwiftUI.Animation { _skeleton() }
    public static func bouncy(duration a0: Swift.Double, extraBounce a1: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public func delay(_ a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public func speed(_ a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public func repeatCount(_ a0: Swift.Int, autoreverses a1: Swift.Bool) -> SwiftUI.Animation { _skeleton() }
    public func repeatForever(autoreverses a0: Swift.Bool) -> SwiftUI.Animation { _skeleton() }
    public func logicallyComplete(after a0: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public static var spring: SwiftUI.Animation { _skeleton() }
    public static func == (_ a0: SwiftUI.Animation, _ a1: SwiftUI.Animation) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public static func spring(response a0: Swift.Double, dampingFraction a1: Swift.Double, blendDuration a2: Swift.Double) -> SwiftUI.Animation { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.Animation: Swift.Equatable {
}

extension SwiftUI.Animation: Swift.CustomStringConvertible {
}

extension SwiftUI.Animation: Swift.CustomDebugStringConvertible {
}

extension SwiftUI.Animation: Swift.Hashable {
}

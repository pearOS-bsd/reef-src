// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Angle (Misc).

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

public struct Angle {
    public var radians: Swift.Double
    public var animatableData: Swift.Double {
        get { _skeleton() }
        set { _skeleton() }
    }
    public static var zero: SwiftUI.Angle { _skeleton() }
    public static func radians(_ a0: Swift.Double) -> SwiftUI.Angle { _skeleton() }
    public var degrees: Swift.Double {
        get { _skeleton() }
        set { _skeleton() }
    }
    public init() { _skeleton() }
    public init(radians a0: Swift.Double) { _skeleton() }
    public init(degrees a0: Swift.Double) { _skeleton() }
    public static func < (_ a0: SwiftUI.Angle, _ a1: SwiftUI.Angle) -> Swift.Bool { _skeleton() }
    public static func degrees(_ a0: Swift.Double) -> SwiftUI.Angle { _skeleton() }
    public static func == (_ a0: SwiftUI.Angle, _ a1: SwiftUI.Angle) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.Angle: SwiftUI.Animatable {
    public typealias AnimatableData = Swift.Double
}

extension SwiftUI.Angle: Swift.Hashable {
}

extension SwiftUI.Angle: Swift.Comparable {
}

extension SwiftUI.Angle: Swift.Equatable {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Gradient (Drawing).

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

public struct Gradient {
    public var stops: [SwiftUI.Gradient.Stop]
    public init(stops a0: [SwiftUI.Gradient.Stop]) { _skeleton() }
    public init(colors a0: [SwiftUI.Color]) { _skeleton() }
    public static func == (_ a0: SwiftUI.Gradient, _ a1: SwiftUI.Gradient) -> Swift.Bool { _skeleton() }
    public struct Stop {
        public var color: SwiftUI.Color
        public var location: CoreGraphics.CGFloat
        public init(color a0: SwiftUI.Color, location a1: CoreGraphics.CGFloat) { _skeleton() }
        public static func == (_ a0: SwiftUI.Gradient.Stop, _ a1: SwiftUI.Gradient.Stop) -> Swift.Bool { _skeleton() }
    }
    public struct ColorSpace {
        let rawValue: Swift.Int
        init(rawValue a0: Swift.Int) { rawValue = a0 }
        public static var device: SwiftUI.Gradient.ColorSpace { ColorSpace(rawValue: 0) }
        public static var perceptual: SwiftUI.Gradient.ColorSpace { ColorSpace(rawValue: 1) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Gradient.ColorSpace, _ a1: SwiftUI.Gradient.ColorSpace) -> Swift.Bool { a0.rawValue == a1.rawValue }
        public func hash(into a0: inout Swift.Hasher) { a0.combine(UInt(bitPattern: rawValue)) }
    }
}

extension SwiftUI.Gradient.Stop: Swift.Equatable {
}

extension SwiftUI.Gradient.ColorSpace: Swift.Hashable {
}

extension SwiftUI.Gradient.ColorSpace: Swift.Equatable {
}

extension SwiftUI.Gradient: Swift.Equatable {
}

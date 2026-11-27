// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// UnitPoint (Misc).

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

public struct UnitPoint {
    public var x: CoreGraphics.CGFloat
    public var y: CoreGraphics.CGFloat
    public static var center: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0.5, y: 0.5) }
    public init() { x = 0; y = 0 }
    public static var zero: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0, y: 0) }
    public static var leading: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0, y: 0.5) }
    public static var trailing: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 1, y: 0.5) }
    public static var top: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0.5, y: 0) }
    public static var bottom: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0.5, y: 1) }
    public static var topLeading: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0, y: 0) }
    public static var topTrailing: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 1, y: 0) }
    public static var bottomLeading: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 0, y: 1) }
    public static var bottomTrailing: SwiftUI.UnitPoint { SwiftUI.UnitPoint(x: 1, y: 1) }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(x); a0.combine(y) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public init(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) { x = a0; y = a1 }
    public static func == (_ a0: SwiftUI.UnitPoint, _ a1: SwiftUI.UnitPoint) -> Swift.Bool { a0.x == a1.x && a0.y == a1.y }
}

extension SwiftUI.UnitPoint: Swift.Hashable {
}

extension SwiftUI.UnitPoint: Swift.Equatable {
}

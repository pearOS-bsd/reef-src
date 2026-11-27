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
    public static var center: SwiftUI.UnitPoint { _skeleton() }
    public init() { _skeleton() }
    public static var zero: SwiftUI.UnitPoint { _skeleton() }
    public static var leading: SwiftUI.UnitPoint { _skeleton() }
    public static var trailing: SwiftUI.UnitPoint { _skeleton() }
    public static var top: SwiftUI.UnitPoint { _skeleton() }
    public static var bottom: SwiftUI.UnitPoint { _skeleton() }
    public static var topLeading: SwiftUI.UnitPoint { _skeleton() }
    public static var topTrailing: SwiftUI.UnitPoint { _skeleton() }
    public static var bottomLeading: SwiftUI.UnitPoint { _skeleton() }
    public static var bottomTrailing: SwiftUI.UnitPoint { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public init(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) { _skeleton() }
    public static func == (_ a0: SwiftUI.UnitPoint, _ a1: SwiftUI.UnitPoint) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.UnitPoint: Swift.Hashable {
}

extension SwiftUI.UnitPoint: Swift.Equatable {
}

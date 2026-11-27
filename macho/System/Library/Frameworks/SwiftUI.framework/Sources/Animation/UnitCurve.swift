// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// UnitCurve (Animation).

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

public struct UnitCurve {
    let p1: CGPoint
    let p2: CGPoint
    public static func bezier(startControlPoint a0: SwiftUI.UnitPoint, endControlPoint a1: SwiftUI.UnitPoint) -> SwiftUI.UnitCurve { _skeleton() }
    public static var linear: SwiftUI.UnitCurve { _skeleton() }
    public static var easeIn: SwiftUI.UnitCurve { _skeleton() }
    public static var easeOut: SwiftUI.UnitCurve { _skeleton() }
    public static var easeInOut: SwiftUI.UnitCurve { _skeleton() }
    public static var circularEaseIn: SwiftUI.UnitCurve { _skeleton() }
    public static var circularEaseOut: SwiftUI.UnitCurve { _skeleton() }
    public static var circularEaseInOut: SwiftUI.UnitCurve { _skeleton() }
    func parameter(`for` a0: Swift.Double) -> Swift.Double { _skeleton() }
    public func value(at a0: Swift.Double) -> Swift.Double { _skeleton() }
    public func velocity(at a0: Swift.Double) -> Swift.Double { _skeleton() }
    public static func == (_ a0: SwiftUI.UnitCurve, _ a1: SwiftUI.UnitCurve) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var inverse: SwiftUI.UnitCurve { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.UnitCurve: Swift.Hashable {
}

extension SwiftUI.UnitCurve: Swift.Equatable {
}

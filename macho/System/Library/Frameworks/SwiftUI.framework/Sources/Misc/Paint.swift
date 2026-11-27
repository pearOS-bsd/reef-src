// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Paint (Misc).

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

enum Paint {
    indirect case color(SwiftUI.Color.Resolved)
    indirect case material(SwiftUI.Color.Resolved)
    indirect case glass(variant: Swift.Int32, tint: SwiftUI.Color.Resolved?, fallback: SwiftUI.Color.Resolved)
    indirect case linear(stops: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)], start: SwiftUI.UnitPoint, end: SwiftUI.UnitPoint)
    indirect case angular(stops: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)], center: SwiftUI.UnitPoint, start: SwiftUI.Angle, end: SwiftUI.Angle)
    indirect case radial(stops: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)], center: SwiftUI.UnitPoint, startRadius: CoreGraphics.CGFloat, endRadius: CoreGraphics.CGFloat)
    case none
    func fill(_ a0: CGPath, `in` a1: CGRect, context a2: CGContext, eoFill a3: Swift.Bool) { _skeleton() }
    func withOpacity(_ a0: Swift.Double) -> SwiftUI.Paint { _skeleton() }
    func drawGradient(`in` a0: CGRect, context a1: CGContext) { _skeleton() }
    func gradient(_ a0: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)]) -> CGGradient? { _skeleton() }
    static func interpolate(_ a0: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)], at a1: CoreGraphics.CGFloat) -> SwiftUI.Color.Resolved { _skeleton() }
    static func same(_ a0: SwiftUI.Paint?, _ a1: SwiftUI.Paint?) -> Swift.Bool { _skeleton() }
}

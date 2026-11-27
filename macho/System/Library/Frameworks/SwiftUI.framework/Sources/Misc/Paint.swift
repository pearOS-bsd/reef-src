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
    func withOpacity(_ a0: Swift.Double) -> SwiftUI.Paint {
        // every colour's opacity times a0 (the stops of gradients too, and a glass's tint and fallback)
        func faded(_ c: SwiftUI.Color.Resolved) -> SwiftUI.Color.Resolved {
            var c = c
            c.opacity *= Float(a0)
            return c
        }
        func fade(_ stops: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)]) -> [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)] {
            stops.map { (faded($0.0), $0.1) }
        }
        switch self {
        case .color(let c): return .color(faded(c))
        case .material(let c): return .material(faded(c))
        case .glass(let variant, let tint, let fallback): return .glass(variant: variant, tint: tint.map(faded), fallback: faded(fallback))
        case .linear(let stops, let start, let end): return .linear(stops: fade(stops), start: start, end: end)
        case .angular(let stops, let center, let start, let end): return .angular(stops: fade(stops), center: center, start: start, end: end)
        case .radial(let stops, let center, let r0, let r1): return .radial(stops: fade(stops), center: center, startRadius: r0, endRadius: r1)
        case .none: return .none
        }
    }
    func drawGradient(`in` a0: CGRect, context a1: CGContext) { _skeleton() }
    func gradient(_ a0: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)]) -> CGGradient? { _skeleton() }
    static func interpolate(_ a0: [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)], at a1: CoreGraphics.CGFloat) -> SwiftUI.Color.Resolved { _skeleton() }
    static func same(_ a0: SwiftUI.Paint?, _ a1: SwiftUI.Paint?) -> Swift.Bool { _skeleton() }
}

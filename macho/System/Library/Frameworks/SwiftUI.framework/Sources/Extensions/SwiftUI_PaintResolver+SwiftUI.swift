// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_PaintResolver.

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

enum PaintResolver {
    /// opacity of the hierarchy levels a style without its own level colours is faded to
    static func hierarchyOpacity(_ a0: Swift.Int) -> Swift.Double {
        [1, 0.5, 0.25, 0.18, 0.1][min(max(a0, 0), 4)]
    }

    static func resolve<A>(_ a0: A, _ a1: SwiftUI.EnvironmentValues, level a2: Swift.Int) -> SwiftUI.Paint where A: SwiftUI.ShapeStyle { _skeleton() }
    static func label(_ a0: Swift.Int, _ a1: SwiftUI.EnvironmentValues) -> SwiftUI.Paint {
        // the label colour of the hierarchy level (primary ... quaternary), resolved for the colour scheme
        if SwiftUI.ViewBuild.debugFlags.contains("nolabel") { return .none }
        let colors = [NSColor.labelColor, NSColor.secondaryLabelColor, NSColor.tertiaryLabelColor, NSColor.quaternaryLabelColor]
        return .color(SwiftUI.Color.Resolved(colors[min(max(a0, 0), 3)], colorScheme: a1[SwiftUI.ColorSchemeKey.self]))
    }
    static func stops(_ a0: SwiftUI.Gradient, _ a1: SwiftUI.EnvironmentValues) -> [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)] { _skeleton() }
    static func backgroundLevel(_ a0: Swift.Int, _ a1: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    static func materialPaint(_ a0: SwiftUI.Material, _ a1: SwiftUI.EnvironmentValues) -> SwiftUI.Paint { _skeleton() }
    static func foreground(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint {
        // the foreground style (its own secondary/tertiary styles, else faded by level), else the foreground colour
        // (faded by level), else the label colour of the level
        if SwiftUI.ViewBuild.tracing, let color = a0[SwiftUI.ForegroundColorKey.self] {
            let box = color.provider
            NSLog("SwiftUI: foreground colour %@ %@ refcount word %@", String(describing: type(of: box)),
                  String(describing: Unmanaged.passUnretained(box).toOpaque()),
                  String(unsafeBitCast(box, to: UnsafePointer<UInt>.self).advanced(by: 1).pointee, radix: 16))
        }
        let debug = SwiftUI.ViewBuild.debugFlags
        if debug.contains("noforeground") { return .none }
        if let style = a0[SwiftUI.ForegroundStyleBoxKey.self] {
            let paint = style.paint(a0, level: a1)
            switch a1 {
            case 0: return paint
            case 1 where style.secondary != nil: return paint
            case 2 where style.tertiary != nil: return paint
            default: return paint.withOpacity(hierarchyOpacity(a1))
            }
        }
        if let color = a0[SwiftUI.ForegroundColorKey.self] {
            if debug.contains("nocolor") { return .none }
            var resolved = color.provider.resolve(in: a0)
            if a1 != 0 { resolved.opacity *= Float(hierarchyOpacity(a1)) }
            return .color(resolved)
        }
        return label(a1, a0)
    }
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MaterialRegions (Drawing).

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

enum MaterialRegions {
    static func cornerRadius(_ a0: CGPath, _ a1: CGRect) -> CoreGraphics.CGFloat { _skeleton() }
    static func note(_ a0: CGPath, color a1: SwiftUI.Color.Resolved, context a2: CGContext) { _skeleton() }
    static func beginClip(_ a0: CGPath, context a1: CGContext) { _skeleton() }
    static func backdrop(context a0: CGContext) -> SwiftUI.MaterialRegions.Backdrop? { _skeleton() }
    static func noteComposited(_ a0: CGPath, context a1: CGContext) { _skeleton() }
    static func takeComposited(since a0: Swift.Int, context a1: CGContext) -> [CGPath] { _skeleton() }
    static var composited: [CGPath] { _skeleton() }
    static var clips: [CGPath] { _skeleton() }
    static var groups: [[SwiftUI.MaterialRegions.Backdrop]] { _skeleton() }
    struct Backdrop {
        let path: CGPath
        let color: SwiftUI.Color.Resolved
    }
}

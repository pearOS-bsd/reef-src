// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CanvasDamage (Drawing).

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

enum CanvasDamage {
    static func visit(_ a0: SwiftUI.Element, transform a1: CGAffineTransform, outset a2: CoreGraphics.CGFloat, effects a3: [SwiftUI.Element], into a4: inout [SwiftUI.DrawnPart]) { _skeleton() }
    struct Key {
        let type: Swift.ObjectIdentifier
        let x: CoreGraphics.CGFloat
        let y: CoreGraphics.CGFloat
        let width: CoreGraphics.CGFloat
        let height: CoreGraphics.CGFloat
    }
}

extension SwiftUI.CanvasDamage.Key: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.CanvasDamage.Key: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

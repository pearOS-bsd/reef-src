// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorMatrix (Drawing).

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

struct ColorMatrix {
    var m: [Swift.Float]
    func then(_ a0: SwiftUI.ColorMatrix) -> SwiftUI.ColorMatrix { _skeleton() }
    static func contrast(_ a0: Swift.Double) -> SwiftUI.ColorMatrix { _skeleton() }
    static func saturation(_ a0: Swift.Double) -> SwiftUI.ColorMatrix { _skeleton() }
    static func brightness(_ a0: Swift.Double) -> SwiftUI.ColorMatrix { _skeleton() }
    static func hueRotation(_ a0: SwiftUI.Angle) -> SwiftUI.ColorMatrix { _skeleton() }
    static func multiply(_ a0: SwiftUI.Color.Resolved) -> SwiftUI.ColorMatrix { _skeleton() }
}

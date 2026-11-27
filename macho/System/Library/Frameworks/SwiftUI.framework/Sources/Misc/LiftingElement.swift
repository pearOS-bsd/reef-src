// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LiftingElement (Misc).

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

protocol LiftingElement: SwiftUI.Element {
    var coveredViews: [NSView] { get }
    func dropLift()
    func lifts(_ a0: SwiftUI.Element) -> Swift.Bool
    var liftedCanvas: SwiftUI.ElementCanvas? { get }
    var liftedPart: SwiftUI.Element { get }
}

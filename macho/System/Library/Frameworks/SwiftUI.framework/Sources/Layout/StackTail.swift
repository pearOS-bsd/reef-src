// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StackTail (Layout).

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

final class StackTail: SwiftUI.Element {
    let part: SwiftUI.FrontElements = SwiftUI.FrontElements()
    var liftedFrom: Swift.Int = 0
    weak var stack: SwiftUI.Element?
    weak var liftedCanvas: SwiftUI.ElementCanvas?
    func lifts(_ a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    var coveredViews: [NSView] { _skeleton() }
    override init() { super.init() }
    deinit {}
}

extension SwiftUI.StackTail: SwiftUI.LiftingElement {
    func dropLift() { _skeleton() }
    var liftedPart: SwiftUI.Element { _skeleton() }
}

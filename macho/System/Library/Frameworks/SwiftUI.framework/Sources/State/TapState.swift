// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TapState (State).

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

class TapState {
    var action: () -> () = _skeletonValue()
    var count: Swift.Int = _skeletonValue()
    weak var element: SwiftUI.Element?
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.TapState: SwiftUI.PressTarget {
    func contains(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    func begin(at a0: CGPoint, event a1: NSEvent) { _skeleton() }
    func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
}

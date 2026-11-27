// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DragSourceSession (Gestures).

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

final class DragSourceSession: NSObject {
    var data: () -> NSItemProvider = _skeletonValue()
    var preview: SwiftUI.AnyView? = _skeletonValue()
    weak var element: SwiftUI.Element?
    var start: CGPoint? = _skeletonValue()
    var started: Swift.Bool = _skeletonValue()
    func beginSession(_ a0: NSEvent) { _skeleton() }
    static func image(of a0: CGRect, `in` a1: NSView) -> NSImage? { _skeleton() }
    deinit {}
}

extension SwiftUI.DragSourceSession: SwiftUI.PressTarget {
    func contains(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    func begin(at a0: CGPoint, event a1: NSEvent) { _skeleton() }
    func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
}

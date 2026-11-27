// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusTracker (Controls).

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

struct FocusTracker {
    weak var host: SwiftUI.ViewHost?
    var hadFocus: Swift.Bool
    var placedInitialFocus: Swift.Bool
    var handledKeys: Swift.Set<Swift.UInt16>
    var lastFocused: SwiftUI.FocusKey?
    func resolveFocus() -> SwiftUI.FocusableElement? { _skeleton() }
    func click(_ a0: SwiftUI.Element?) { _skeleton() }
    static func firstKeyView(`in` a0: NSView) -> NSView? { _skeleton() }
    func sync(canvas a0: SwiftUI.ElementCanvas?) { _skeleton() }
    func placeInitialTextFocus(_ a0: SwiftUI.Element, canvas a1: SwiftUI.ElementCanvas, window a2: NSWindow) { _skeleton() }
    func textViewBeganEditing(_ a0: NSView) { _skeleton() }
    func responderChanged(_ a0: NSResponder?, canvas a1: SwiftUI.ElementCanvas) { _skeleton() }
    func focusedValues(responder a0: NSResponder?) -> (values: SwiftUI.FocusedValues, focus: Swift.AnyHashable?) { _skeleton() }
    func performExitCommand() -> Swift.Bool { _skeleton() }
    func dispatchKey(_ a0: NSEvent, phase a1: SwiftUI.KeyPress.Phases, from a2: SwiftUI.Element?) -> Swift.Bool { _skeleton() }
}

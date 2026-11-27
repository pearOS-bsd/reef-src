// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_MenuContent.

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

enum MenuContent {
    static func menu(_ a0: [SwiftUI.MenuEntry], title a1: Swift.String) -> NSMenu { _skeleton() }
    static func entries<A>(_ a0: A, _ a1: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] where A: SwiftUI.View { _skeleton() }
    static func label<A>(_ a0: A) -> (Swift.String, NSImage?) where A: SwiftUI.View { _skeleton() }
    static func collectLabel(_ a0: Any, texts a1: inout [Swift.String], image a2: inout NSImage?) { _skeleton() }
    static func item(_ a0: SwiftUI.MenuEntry) -> NSMenuItem { _skeleton() }
    static func nsImage(_ a0: SwiftUI.Image) -> NSImage? { _skeleton() }
}

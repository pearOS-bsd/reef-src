// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MenuEntry (Navigation).

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

enum MenuEntry {
    indirect case item(title: Swift.String, image: NSImage?, shortcut: SwiftUI.KeyboardShortcut?, enabled: Swift.Bool, checked: Swift.Bool?, action: () -> ())
    indirect case submenu(title: Swift.String, image: NSImage?, entries: [SwiftUI.MenuEntry])
    indirect case header(Swift.String)
    case separator
}

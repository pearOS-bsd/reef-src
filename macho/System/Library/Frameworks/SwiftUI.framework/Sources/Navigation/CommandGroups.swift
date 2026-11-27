// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CommandGroups (Navigation).

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

struct CommandGroups {
    var groups: [SwiftUI.CommandGroups.Group]
    var menus: [SwiftUI.CommandGroups.CustomMenu]
    func section(_ a0: SwiftUI.CommandGroupPlacement, into a1: NSMenu, standard a2: () -> [NSMenuItem]) { _skeleton() }
    func collect(_ a0: Any) { _skeleton() }
    init(_ a0: [any SwiftUI.Commands]) { _skeleton() }
    struct CustomMenu {
        let name: SwiftUI.Text
        let entries: [SwiftUI.MenuEntry]
    }
    struct Group {
        let placement: Swift.String
        let position: Swift.Int
        let entries: [SwiftUI.MenuEntry]
    }
}

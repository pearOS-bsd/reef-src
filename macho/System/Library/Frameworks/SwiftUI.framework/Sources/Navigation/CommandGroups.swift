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
    func collect(_ a0: Any) { _skeleton() }
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

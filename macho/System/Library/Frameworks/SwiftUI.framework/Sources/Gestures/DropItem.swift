// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropItem (Gestures).

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

struct DropItem {
    let item: NSPasteboardItem
    func conforms(to a0: [UniformTypeIdentifiers.UTType]) -> Swift.Bool { _skeleton() }
    func provider() -> NSItemProvider { _skeleton() }
}

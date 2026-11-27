// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableRowEntry (Navigation).

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

struct TableRowEntry {
    let id: Swift.AnyHashable
    let value: Any
    var payload: (() -> Any)?
    var insertion: ((target: SwiftUI.TableRowInsertion, offset: Swift.Int))?
    var sectionHeader: SwiftUI.AnyView?
    var isOutline: Swift.Bool
    var children: [SwiftUI.TableRowEntry]?
}

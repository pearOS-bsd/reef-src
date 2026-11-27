// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListRowDrag (Gestures).

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

struct ListRowDrag {
    var move: ((group: SwiftUI.ListMoveGroup, offset: Swift.Int))?
    var payload: (() -> Any)?
    var insertion: ((target: SwiftUI.TableRowInsertion, offset: Swift.Int))?
    static func of(_ a0: SwiftUI.Element) -> SwiftUI.ListRowDrag? { _skeleton() }
    static var key: Swift.ObjectIdentifier { _skeleton() }
}

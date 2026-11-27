// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableColumnEntry (Navigation).

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

struct TableColumnEntry {
    let title: SwiftUI.AnyView
    let cell: (Any) -> SwiftUI.AnyView
    let minWidth: CoreGraphics.CGFloat?
    let idealWidth: CoreGraphics.CGFloat?
    let maxWidth: CoreGraphics.CGFloat?
    let alignment: SwiftUI.TableColumnAlignment
    let comparator: Any?
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListKind (Lists).

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

enum ListKind {
    case inset
    case plain
    case bordered
    case sidebar
    init(_ a0: Any?) { _skeleton() }
}

extension SwiftUI.ListKind: Swift.Hashable {
}

extension SwiftUI.ListKind: Swift.Equatable {
}

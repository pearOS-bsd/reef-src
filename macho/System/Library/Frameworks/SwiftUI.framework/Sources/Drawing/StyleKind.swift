// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StyleKind (Drawing).

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

enum StyleKind {
    case picker
    case list
    case table
    case menu
    case textField
    case label
    case labeledContent
    case form
    case groupBox
    case controlGroup
    case tabView
    case datePicker
    case disclosureGroup
    case gauge
    init?(_ a0: Any) { _skeleton() }
}

extension SwiftUI.StyleKind: Swift.Hashable {
}

extension SwiftUI.StyleKind: Swift.Equatable {
}

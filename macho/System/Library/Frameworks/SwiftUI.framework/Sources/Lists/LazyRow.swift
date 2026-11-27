// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyRow (Lists).

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

struct LazyRow {
    let id: Swift.AnyHashable?
    let key: [Swift.AnyHashable]
    let build: () -> [SwiftUI.Element]
    var role: SwiftUI.LazyRow.Role
    var section: [Swift.AnyHashable]?
    enum Role {
        case row
        case header
        case footer
    }
}

extension SwiftUI.LazyRow.Role: Swift.Hashable {
}

extension SwiftUI.LazyRow.Role: Swift.Equatable {
}

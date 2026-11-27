// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TypeKey (Misc).

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

struct TypeKey {
    let type: Any.Type
}

extension SwiftUI.TypeKey: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { a0.type == a1.type }
}

extension SwiftUI.TypeKey: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { a0.combine(ObjectIdentifier(type)) }
}

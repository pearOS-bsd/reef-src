// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusKey (Controls).

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

struct FocusKey {
    let scope: Swift.ObjectIdentifier
    let path: [Swift.AnyHashable]
}

extension SwiftUI.FocusKey: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.FocusKey: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

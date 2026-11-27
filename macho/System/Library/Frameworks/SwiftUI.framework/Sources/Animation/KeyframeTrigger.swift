// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeTrigger (Animation).

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

struct KeyframeTrigger {
    let value: Any
    let equals: (Any) -> Swift.Bool
    init<A>(_ a0: A) where A: Swift.Equatable { _skeleton() }
}

extension SwiftUI.KeyframeTrigger: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.KeyframeTrigger: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

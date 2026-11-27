// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EnvironmentBox: one value of EnvironmentValues, with whether its key is the app's own.

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

class EnvironmentBox<A> {
    let value: A
    let isCustom: Swift.Bool
    init(_ a0: A, isCustom a1: Swift.Bool) {
        value = a0
        isCustom = a1
    }
    final func sameValue(`as` a0: Any) -> Swift.Bool {
        guard let other = a0 as? SwiftUI.EnvironmentBox<A> else { return false }
        return SwiftUI.GraphCompare.equal(value, other.value)
    }
    deinit {}
}

extension SwiftUI.EnvironmentBox: SwiftUI.EnvironmentBoxComparing {
}

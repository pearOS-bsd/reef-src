// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EnvironmentBox (Environment).

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
    let value: A = _skeletonValue()
    let isCustom: Swift.Bool = _skeletonValue()
    init(_ a0: A, isCustom a1: Swift.Bool) { _skeleton() }
    final func sameValue(`as` a0: Any) -> Swift.Bool { _skeleton() }
    deinit {}
}

extension SwiftUI.EnvironmentBox: SwiftUI.EnvironmentBoxComparing {
}

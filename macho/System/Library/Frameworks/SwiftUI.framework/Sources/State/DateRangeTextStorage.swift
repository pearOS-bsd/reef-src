// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DateRangeTextStorage (State).

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

class DateRangeTextStorage: SwiftUI.AnyTextStorage {
    let lower: Foundation.Date = _skeletonValue()
    let upper: Foundation.Date = _skeletonValue()
    override var plainString: Swift.String { _skeleton() }
    init(lower a0: Foundation.Date, upper a1: Foundation.Date) { _skeleton() }
    deinit {}
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DateTextStorage (State).

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

class DateTextStorage: SwiftUI.AnyTextStorage {
    let date: Foundation.Date = _skeletonValue()
    let style: SwiftUI.Text.DateStyle = _skeletonValue()
    override var plainString: Swift.String { _skeleton() }
    init(date a0: Foundation.Date, style a1: SwiftUI.Text.DateStyle) { _skeleton() }
    deinit {}
}

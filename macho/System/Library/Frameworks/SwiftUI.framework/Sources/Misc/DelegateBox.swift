// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DelegateBox (Misc).

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

class DelegateBox {
    let make: () -> NSObject = _skeletonValue()
    var made: NSObject? = _skeletonValue()
    init(make a0: @escaping () -> NSObject) { _skeleton() }
    deinit {}
}

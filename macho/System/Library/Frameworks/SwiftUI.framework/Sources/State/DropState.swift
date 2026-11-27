// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropState (State).

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

class DropState {
    var types: [UniformTypeIdentifiers.UTType] = _skeletonValue()
    var delegate: (any SwiftUI.DropDelegate)? = _skeletonValue()
    init() { _skeleton() }
    deinit {}
}

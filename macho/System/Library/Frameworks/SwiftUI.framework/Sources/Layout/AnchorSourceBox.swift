// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnchorSourceBox (Layout).

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

class AnchorSourceBox<A>: SwiftUI.AnchorBoxBase {
    let make: (CGRect) -> A = _skeletonValue()
    init(_ a0: @escaping (CGRect) -> A) { _skeleton() }
    deinit {}
}

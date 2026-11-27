// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MaterialRegions (Drawing).

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

enum MaterialRegions {
    static var composited: [CGPath] { _skeleton() }
    static var clips: [CGPath] { _skeleton() }
    static var groups: [[SwiftUI.MaterialRegions.Backdrop]] { _skeleton() }
    struct Backdrop {
        let path: CGPath
        let color: SwiftUI.Color.Resolved
    }
}

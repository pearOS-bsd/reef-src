// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AsyncImageLoader (Images).

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

class AsyncImageLoader {
    var url: Foundation.URL? = _skeletonValue()
    let phase: SwiftUI.StoredLocation<SwiftUI.AsyncImagePhase> = _skeletonValue()
    final func load(_ a0: Foundation.URL?) { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

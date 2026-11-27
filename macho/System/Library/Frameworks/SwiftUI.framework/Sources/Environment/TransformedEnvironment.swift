// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TransformedEnvironment (Environment).

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

class TransformedEnvironment {
    var input: SwiftUI.EnvironmentValues? = _skeletonValue()
    var output: SwiftUI.EnvironmentValues? = _skeletonValue()
    var modifier: Any? = _skeletonValue()
    init() { _skeleton() }
    deinit {}
}

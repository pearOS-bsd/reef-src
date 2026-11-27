// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// InspectorColumnWidth (Navigation).

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

struct InspectorColumnWidth {
    var min: CoreGraphics.CGFloat?
    var ideal: CoreGraphics.CGFloat
    var max: CoreGraphics.CGFloat?
    static func == (_ a0: SwiftUI.InspectorColumnWidth, _ a1: SwiftUI.InspectorColumnWidth) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.InspectorColumnWidth: Swift.Equatable {
}

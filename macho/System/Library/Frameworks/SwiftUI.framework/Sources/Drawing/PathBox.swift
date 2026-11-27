// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PathBox (Drawing).

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

public class PathBox {
    let elements: [SwiftUI.Path.Element] = _skeletonValue()
    public static func == (_ a0: SwiftUI.PathBox, _ a1: SwiftUI.PathBox) -> Swift.Bool { _skeleton() }
    init(_ a0: [SwiftUI.Path.Element]) { _skeleton() }
    deinit {}
}

extension SwiftUI.PathBox: Swift.Equatable {
}

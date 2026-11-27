// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpacerSizing (Layout).

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

public struct SpacerSizing {
    let rawValue: Swift.Int
    public static var flexible: SwiftUI.SpacerSizing { SwiftUI.SpacerSizing(rawValue: 0) }
    public static var fixed: SwiftUI.SpacerSizing { SwiftUI.SpacerSizing(rawValue: 1) }
    public static func == (_ a0: SwiftUI.SpacerSizing, _ a1: SwiftUI.SpacerSizing) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.SpacerSizing: Swift.Equatable {
}

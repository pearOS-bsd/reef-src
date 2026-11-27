// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationContentInteraction (Navigation).

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

public struct PresentationContentInteraction {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.PresentationContentInteraction { SwiftUI.PresentationContentInteraction(rawValue: 0) }
    public static var resizes: SwiftUI.PresentationContentInteraction { SwiftUI.PresentationContentInteraction(rawValue: 1) }
    public static var scrolls: SwiftUI.PresentationContentInteraction { SwiftUI.PresentationContentInteraction(rawValue: 2) }
    public static func == (_ a0: SwiftUI.PresentationContentInteraction, _ a1: SwiftUI.PresentationContentInteraction) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.PresentationContentInteraction: Swift.Equatable {
}

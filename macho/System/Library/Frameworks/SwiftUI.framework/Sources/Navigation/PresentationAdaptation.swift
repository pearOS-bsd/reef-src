// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationAdaptation (Navigation).

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

public struct PresentationAdaptation {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.PresentationAdaptation { SwiftUI.PresentationAdaptation(rawValue: 0) }
    public static var none: SwiftUI.PresentationAdaptation { SwiftUI.PresentationAdaptation(rawValue: 1) }
    public static var popover: SwiftUI.PresentationAdaptation { SwiftUI.PresentationAdaptation(rawValue: 2) }
    public static var sheet: SwiftUI.PresentationAdaptation { SwiftUI.PresentationAdaptation(rawValue: 3) }
    public static var fullScreenCover: SwiftUI.PresentationAdaptation { SwiftUI.PresentationAdaptation(rawValue: 4) }
    public static func == (_ a0: SwiftUI.PresentationAdaptation, _ a1: SwiftUI.PresentationAdaptation) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.PresentationAdaptation: Swift.Equatable {
}

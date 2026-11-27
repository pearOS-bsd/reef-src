// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityTextContentType (Text).

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

public struct AccessibilityTextContentType {
    let rawValue: Swift.Int
    public static var plain: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 0) }
    public static var console: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 1) }
    public static var fileSystem: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 2) }
    public static var messaging: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 3) }
    public static var narrative: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 4) }
    public static var sourceCode: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 5) }
    public static var spreadsheet: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 6) }
    public static var wordProcessing: SwiftUI.AccessibilityTextContentType { SwiftUI.AccessibilityTextContentType(rawValue: 7) }
}

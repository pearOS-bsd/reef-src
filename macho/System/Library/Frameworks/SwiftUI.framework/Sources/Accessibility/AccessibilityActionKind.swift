// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityActionKind (Accessibility).

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

public struct AccessibilityActionKind {
    let rawValue: Swift.Int
    init(rawValue a0: Swift.Int) { rawValue = a0 }
    public init(named a0: SwiftUI.Text) { _skeleton() }
    public static var escape: SwiftUI.AccessibilityActionKind { SwiftUI.AccessibilityActionKind(rawValue: 1) }
    public static var `default`: SwiftUI.AccessibilityActionKind { SwiftUI.AccessibilityActionKind(rawValue: 0) }
    public static var magicTap: SwiftUI.AccessibilityActionKind { SwiftUI.AccessibilityActionKind(rawValue: 2) }
    public static var showMenu: SwiftUI.AccessibilityActionKind { SwiftUI.AccessibilityActionKind(rawValue: 4) }
    public static var delete: SwiftUI.AccessibilityActionKind { SwiftUI.AccessibilityActionKind(rawValue: 3) }
    public static func == (_ a0: SwiftUI.AccessibilityActionKind, _ a1: SwiftUI.AccessibilityActionKind) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.AccessibilityActionKind: Swift.Equatable {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityTechnologySet (Accessibility).

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

struct AccessibilityTechnologySet {
    let rawValue: Swift.UInt16
}

extension SwiftUI.AccessibilityTechnologySet: Swift.ExpressibleByArrayLiteral {
    typealias ArrayLiteralElement = SwiftUI.AccessibilityTechnologySet
}

extension SwiftUI.AccessibilityTechnologySet: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.AccessibilityTechnologySet: Swift.RawRepresentable {
    typealias RawValue = Swift.UInt16
}

extension SwiftUI.AccessibilityTechnologySet: Swift.SetAlgebra {
    typealias Element = SwiftUI.AccessibilityTechnologySet
}

extension SwiftUI.AccessibilityTechnologySet: Swift.OptionSet {
}

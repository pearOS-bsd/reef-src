// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityHeadingLevel (Accessibility).

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

public enum AccessibilityHeadingLevel {
    case unspecified
    case h1
    case h2
    case h3
    case h4
    case h5
    case h6
    public init?(rawValue a0: Swift.UInt) { _skeleton() }
    public var rawValue: Swift.UInt { _skeleton() }
}

extension SwiftUI.AccessibilityHeadingLevel: Swift.Equatable {
}

extension SwiftUI.AccessibilityHeadingLevel: Swift.Hashable {
}

extension SwiftUI.AccessibilityHeadingLevel: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt
}

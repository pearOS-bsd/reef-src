// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextSelectionAffinity (Text).

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

public enum TextSelectionAffinity {
    case automatic
    case upstream
    case downstream
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.TextSelectionAffinity, _ a1: SwiftUI.TextSelectionAffinity) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.TextSelectionAffinity: Swift.Equatable {
}

extension SwiftUI.TextSelectionAffinity: Swift.Hashable {
}

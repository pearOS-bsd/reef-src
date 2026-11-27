// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorRenderingMode (Drawing).

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

public enum ColorRenderingMode {
    case nonLinear
    case linear
    case extendedLinear
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.ColorRenderingMode, _ a1: SwiftUI.ColorRenderingMode) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ColorRenderingMode: Swift.Equatable {
}

extension SwiftUI.ColorRenderingMode: Swift.Hashable {
}

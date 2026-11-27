// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RoundedCornerStyle (Drawing).

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

public enum RoundedCornerStyle {
    case circular
    case continuous
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.RoundedCornerStyle, _ a1: SwiftUI.RoundedCornerStyle) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.RoundedCornerStyle: Swift.Equatable {
}

extension SwiftUI.RoundedCornerStyle: Swift.Hashable {
}

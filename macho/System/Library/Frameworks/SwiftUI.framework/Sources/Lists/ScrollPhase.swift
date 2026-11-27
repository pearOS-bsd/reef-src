// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollPhase (Lists).

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

public enum ScrollPhase {
    case idle
    case tracking
    case interacting
    case decelerating
    case animating
    public var isScrolling: Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.ScrollPhase, _ a1: SwiftUI.ScrollPhase) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ScrollPhase: Swift.Hashable {
}

extension SwiftUI.ScrollPhase: Swift.Equatable {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HoverPhase (Gestures).

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

public enum HoverPhase {
    case active(CGPoint)
    case ended
    public static func == (_ a0: SwiftUI.HoverPhase, _ a1: SwiftUI.HoverPhase) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.HoverPhase: Swift.Equatable {
}

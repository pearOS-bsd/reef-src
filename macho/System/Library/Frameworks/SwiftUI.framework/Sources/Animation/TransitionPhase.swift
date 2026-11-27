// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TransitionPhase (Animation).

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

public enum TransitionPhase {
    case willAppear
    case identity
    case didDisappear
    public var isIdentity: Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.TransitionPhase, _ a1: SwiftUI.TransitionPhase) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.TransitionPhase: Swift.Equatable {
}

extension SwiftUI.TransitionPhase: Swift.Hashable {
}

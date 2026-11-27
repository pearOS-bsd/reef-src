// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScenePhase (Scenes).

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

public enum ScenePhase {
    case background
    case inactive
    case active
    public static func < (_ a0: SwiftUI.ScenePhase, _ a1: SwiftUI.ScenePhase) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ScenePhase, _ a1: SwiftUI.ScenePhase) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
}

extension SwiftUI.ScenePhase: Swift.Equatable {
}

extension SwiftUI.ScenePhase: Swift.Comparable {
}

extension SwiftUI.ScenePhase: Swift.Hashable {
}

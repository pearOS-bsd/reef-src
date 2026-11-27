// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlActiveState (State).

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

public enum ControlActiveState {
    case key
    case active
    case inactive
    public static var allCases: [SwiftUI.ControlActiveState] { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ControlActiveState, _ a1: SwiftUI.ControlActiveState) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
}

extension SwiftUI.ControlActiveState: Swift.Hashable {
}

extension SwiftUI.ControlActiveState: Swift.Equatable {
}

extension SwiftUI.ControlActiveState: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.ControlActiveState]
}

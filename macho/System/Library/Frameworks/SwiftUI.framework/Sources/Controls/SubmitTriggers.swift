// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SubmitTriggers (Controls).

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

public struct SubmitTriggers {
    public let rawValue: Swift.Int
    public init(rawValue a0: Swift.Int) { rawValue = a0 }
    public static var text: SwiftUI.SubmitTriggers { _skeleton() }
    public static var search: SwiftUI.SubmitTriggers { _skeleton() }
}

extension SwiftUI.SubmitTriggers: Swift.OptionSet {
    public typealias Element = SwiftUI.SubmitTriggers
}

extension SwiftUI.SubmitTriggers: Swift.SetAlgebra {
}

extension SwiftUI.SubmitTriggers: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.SubmitTriggers: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.SubmitTriggers: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.SubmitTriggers
}

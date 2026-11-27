// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GestureMask (Gestures).

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

public struct GestureMask {
    public let rawValue: Swift.UInt32
    public static var all: SwiftUI.GestureMask { _skeleton() }
    public init(rawValue a0: Swift.UInt32) { rawValue = a0 }
    public static var none: SwiftUI.GestureMask { _skeleton() }
    public static var gesture: SwiftUI.GestureMask { _skeleton() }
    public static var subviews: SwiftUI.GestureMask { _skeleton() }
}

extension SwiftUI.GestureMask: Swift.OptionSet {
    public typealias Element = SwiftUI.GestureMask
}

extension SwiftUI.GestureMask: Swift.SetAlgebra {
}

extension SwiftUI.GestureMask: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.GestureMask: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.GestureMask: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.GestureMask
}

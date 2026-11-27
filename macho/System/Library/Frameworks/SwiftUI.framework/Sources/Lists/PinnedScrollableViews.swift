// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PinnedScrollableViews (Lists).

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

public struct PinnedScrollableViews {
    public let rawValue: Swift.UInt32
    public init(rawValue a0: Swift.UInt32) { rawValue = a0 }
    public static var sectionHeaders: SwiftUI.PinnedScrollableViews { _skeleton() }
    public static var sectionFooters: SwiftUI.PinnedScrollableViews { _skeleton() }
}

extension SwiftUI.PinnedScrollableViews: Swift.OptionSet {
    public typealias Element = SwiftUI.PinnedScrollableViews
}

extension SwiftUI.PinnedScrollableViews: Swift.SetAlgebra {
}

extension SwiftUI.PinnedScrollableViews: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.PinnedScrollableViews: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.PinnedScrollableViews: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.PinnedScrollableViews
}

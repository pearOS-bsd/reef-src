// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentShapeKinds (Drawing).

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

public struct ContentShapeKinds {
    public let rawValue: Swift.Int
    public init(rawValue a0: Swift.Int) { rawValue = a0 }
    public static var interaction: SwiftUI.ContentShapeKinds { _skeleton() }
    public static var dragPreview: SwiftUI.ContentShapeKinds { _skeleton() }
    public static var contextMenuPreview: SwiftUI.ContentShapeKinds { _skeleton() }
    public static var hoverEffect: SwiftUI.ContentShapeKinds { _skeleton() }
    public static var focusEffect: SwiftUI.ContentShapeKinds { _skeleton() }
    public static var accessibility: SwiftUI.ContentShapeKinds { _skeleton() }
}

extension SwiftUI.ContentShapeKinds: Swift.OptionSet {
    public typealias Element = SwiftUI.ContentShapeKinds
}

extension SwiftUI.ContentShapeKinds: Swift.SetAlgebra {
}

extension SwiftUI.ContentShapeKinds: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.ContentShapeKinds: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.ContentShapeKinds: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.ContentShapeKinds
}

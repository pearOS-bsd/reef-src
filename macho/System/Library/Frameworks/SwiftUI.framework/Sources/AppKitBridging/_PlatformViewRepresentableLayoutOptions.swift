// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _PlatformViewRepresentableLayoutOptions (AppKitBridging).

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

public struct _PlatformViewRepresentableLayoutOptions {
    public let rawValue: Swift.Int
    public init(rawValue a0: Swift.Int) { _skeleton() }
}

extension SwiftUI._PlatformViewRepresentableLayoutOptions: Swift.OptionSet {
    public typealias Element = SwiftUI._PlatformViewRepresentableLayoutOptions
}

extension SwiftUI._PlatformViewRepresentableLayoutOptions: Swift.SetAlgebra {
}

extension SwiftUI._PlatformViewRepresentableLayoutOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI._PlatformViewRepresentableLayoutOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._PlatformViewRepresentableLayoutOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI._PlatformViewRepresentableLayoutOptions
}

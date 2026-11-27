// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContainerBackgroundPlacement (Layout).

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

public struct ContainerBackgroundPlacement {
    let rawValue: Swift.Int
    public init(_rawValue a0: Swift.Int) { _skeleton() }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ContainerBackgroundPlacement, _ a1: SwiftUI.ContainerBackgroundPlacement) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.ContainerBackgroundPlacement: Swift.Hashable {
}

extension SwiftUI.ContainerBackgroundPlacement: Swift.Equatable {
}

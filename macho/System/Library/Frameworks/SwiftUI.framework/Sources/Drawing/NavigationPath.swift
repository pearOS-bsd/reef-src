// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationPath (Drawing).

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

public struct NavigationPath {
    var elements: [Swift.AnyHashable]
    public init() { _skeleton() }
    public init<A>(_ a0: A) where A: Swift.Sequence, A.Element: Swift.Hashable { _skeleton() }
    public var count: Swift.Int { _skeleton() }
    public var isEmpty: Swift.Bool { _skeleton() }
    public func append<A>(_ a0: A) where A: Swift.Hashable { _skeleton() }
    public func removeLast(_ a0: Swift.Int) { _skeleton() }
    public static func == (_ a0: SwiftUI.NavigationPath, _ a1: SwiftUI.NavigationPath) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.NavigationPath: Swift.Equatable {
}

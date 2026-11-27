// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AlignmentKey (Layout).

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

public struct AlignmentKey {
    public let bits: Swift.UInt
    func defaultValue(width a0: CoreGraphics.CGFloat, height a1: CoreGraphics.CGFloat) -> CoreGraphics.CGFloat { _skeleton() }
    public static func == (_ a0: SwiftUI.AlignmentKey, _ a1: SwiftUI.AlignmentKey) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    static var registry: [Swift.UInt: any SwiftUI.AlignmentID.Type] { _skeleton() }
}

extension SwiftUI.AlignmentKey: Swift.Hashable {
}

extension SwiftUI.AlignmentKey: Swift.Equatable {
}

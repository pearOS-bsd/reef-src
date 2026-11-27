// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AlignmentKey: the identity of an alignment guide (its AlignmentID type and axis) and the registry of the IDs.

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
    /// bits: the identity of the alignment ID type, shifted left once, with the axis (0 horizontal, 1 vertical) in bit 0
    init(id: any SwiftUI.AlignmentID.Type, axis: SwiftUI.Axis) {
        bits = UInt(bitPattern: ObjectIdentifier(id).hashValue) << 1 | (axis == .vertical ? 1 : 0)
        AlignmentKey.registry[bits] = id
    }
    func defaultValue(width a0: CoreGraphics.CGFloat, height a1: CoreGraphics.CGFloat) -> CoreGraphics.CGFloat {
        if let id = AlignmentKey.registry[bits] {
            return id.defaultValue(in: SwiftUI.ViewDimensions(width: a0, height: a1))
        }
        // an unknown key: the middle of its axis
        return (bits & 1 != 0 ? a1 : a0) * 0.5
    }
    public static func == (_ a0: SwiftUI.AlignmentKey, _ a1: SwiftUI.AlignmentKey) -> Swift.Bool { a0.bits == a1.bits }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(bits) }
    public var hashValue: Swift.Int { bits.hashValue }
    static var registry: [Swift.UInt: any SwiftUI.AlignmentID.Type] = [:]
}

extension SwiftUI.AlignmentKey: Swift.Hashable {
}

extension SwiftUI.AlignmentKey: Swift.Equatable {
}

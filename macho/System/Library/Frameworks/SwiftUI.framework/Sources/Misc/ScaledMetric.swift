// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScaledMetric (Misc).

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

@propertyWrapper public struct ScaledMetric<A> where A: Swift.BinaryFloatingPoint {
    let value: A
    public init(wrappedValue a0: A, relativeTo a1: SwiftUI.Font.TextStyle) { _skeleton() }
    public var wrappedValue: A { _skeleton() }
    public init(wrappedValue a0: A) { _skeleton() }
}

extension SwiftUI.ScaledMetric: SwiftUI.DynamicProperty {
}

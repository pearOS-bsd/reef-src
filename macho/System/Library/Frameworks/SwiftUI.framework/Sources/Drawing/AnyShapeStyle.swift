// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyShapeStyle (Drawing).

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

public struct AnyShapeStyle {
    public var storage: SwiftUI.AnyShapeStyleBox
    public init<A>(_ a0: A) where A: SwiftUI.ShapeStyle { _skeleton() }
}

extension SwiftUI.AnyShapeStyle: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}

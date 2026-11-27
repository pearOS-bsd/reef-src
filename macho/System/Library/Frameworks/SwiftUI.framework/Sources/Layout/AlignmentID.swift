// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AlignmentID (Layout).

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

public protocol AlignmentID {
    static func _combineExplicit(childValue a0: CoreGraphics.CGFloat, _ a1: Swift.Int, into a2: inout CoreGraphics.CGFloat?)
    static func defaultValue(`in` a0: SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat
}

extension SwiftUI.AlignmentID {
    public static func _combineExplicit(childValue a0: CoreGraphics.CGFloat, _ a1: Swift.Int, into a2: inout CoreGraphics.CGFloat?) { _skeleton() }
}

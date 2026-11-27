// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProjectionTransform (Misc).

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

public struct ProjectionTransform {
    public var m11: CoreGraphics.CGFloat
    public var m12: CoreGraphics.CGFloat
    public var m13: CoreGraphics.CGFloat
    public var m21: CoreGraphics.CGFloat
    public var m22: CoreGraphics.CGFloat
    public var m23: CoreGraphics.CGFloat
    public var m31: CoreGraphics.CGFloat
    public var m32: CoreGraphics.CGFloat
    public var m33: CoreGraphics.CGFloat
    public init(_ a0: CGAffineTransform) { _skeleton() }
    public init() { _skeleton() }
    public static func == (_ a0: SwiftUI.ProjectionTransform, _ a1: SwiftUI.ProjectionTransform) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ProjectionTransform: Swift.Equatable {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StrokeStyle (Drawing).

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

public struct StrokeStyle {
    public var lineWidth: CoreGraphics.CGFloat
    public var lineCap: CGLineCap
    public var lineJoin: CGLineJoin
    public var miterLimit: CoreGraphics.CGFloat
    public var dash: [CoreGraphics.CGFloat]
    public var dashPhase: CoreGraphics.CGFloat
    public init(lineWidth a0: CoreGraphics.CGFloat, lineCap a1: CGLineCap, lineJoin a2: CGLineJoin, miterLimit a3: CoreGraphics.CGFloat, dash a4: [CoreGraphics.CGFloat], dashPhase a5: CoreGraphics.CGFloat) { _skeleton() }
    public static func == (_ a0: SwiftUI.StrokeStyle, _ a1: SwiftUI.StrokeStyle) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.StrokeStyle: Swift.Equatable {
}

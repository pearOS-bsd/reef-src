// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EdgeInsets (Layout).

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

public struct EdgeInsets {
    public var top: CoreGraphics.CGFloat
    public var leading: CoreGraphics.CGFloat
    public var bottom: CoreGraphics.CGFloat
    public var trailing: CoreGraphics.CGFloat
    public init(top a0: CoreGraphics.CGFloat, leading a1: CoreGraphics.CGFloat, bottom a2: CoreGraphics.CGFloat, trailing a3: CoreGraphics.CGFloat) { _skeleton() }
    public init() { _skeleton() }
    public init(_all a0: CoreGraphics.CGFloat) { _skeleton() }
    public static func == (_ a0: SwiftUI.EdgeInsets, _ a1: SwiftUI.EdgeInsets) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.EdgeInsets: Swift.Equatable {
}

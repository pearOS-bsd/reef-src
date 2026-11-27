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
    public init(top a0: CoreGraphics.CGFloat, leading a1: CoreGraphics.CGFloat, bottom a2: CoreGraphics.CGFloat, trailing a3: CoreGraphics.CGFloat) {
        top = a0
        leading = a1
        bottom = a2
        trailing = a3
    }
    public init() {
        top = 0
        leading = 0
        bottom = 0
        trailing = 0
    }
    public init(_all a0: CoreGraphics.CGFloat) {
        top = a0
        leading = a0
        bottom = a0
        trailing = a0
    }
    public static func == (_ a0: SwiftUI.EdgeInsets, _ a1: SwiftUI.EdgeInsets) -> Swift.Bool {
        a0.top == a1.top && a0.leading == a1.leading && a0.bottom == a1.bottom && a0.trailing == a1.trailing
    }
}

extension SwiftUI.EdgeInsets: Swift.Equatable {
}

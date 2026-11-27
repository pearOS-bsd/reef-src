// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GridLayout (Layout).

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

public struct GridLayout {
    public var alignment: SwiftUI.Alignment
    public var horizontalSpacing: CoreGraphics.CGFloat?
    public var verticalSpacing: CoreGraphics.CGFloat?
    public init(alignment a0: SwiftUI.Alignment, horizontalSpacing a1: CoreGraphics.CGFloat?, verticalSpacing a2: CoreGraphics.CGFloat?) { _skeleton() }
}

extension SwiftUI.GridLayout: SwiftUI._VariadicView_UnaryViewRoot {
}

extension SwiftUI.GridLayout: SwiftUI._VariadicView_ViewRoot {
}

extension SwiftUI.GridLayout: SwiftUI._VariadicView_Root {
}

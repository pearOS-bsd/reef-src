// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _HStackLayout (Layout).

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

public struct _HStackLayout {
    public var alignment: SwiftUI.VerticalAlignment
    public var spacing: CoreGraphics.CGFloat?
    public init(alignment a0: SwiftUI.VerticalAlignment, spacing a1: CoreGraphics.CGFloat?) { _skeleton() }
}

extension SwiftUI._HStackLayout: SwiftUI._VariadicView_UnaryViewRoot {
}

extension SwiftUI._HStackLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._HStackLayout: SwiftUI._VariadicView_ViewRoot {
}

extension SwiftUI._HStackLayout: SwiftUI._VariadicView_Root {
}

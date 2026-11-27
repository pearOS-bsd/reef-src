// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _VStackLayout (Layout).

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

public struct _VStackLayout {
    public var alignment: SwiftUI.HorizontalAlignment
    public var spacing: CoreGraphics.CGFloat?
    public init(alignment a0: SwiftUI.HorizontalAlignment, spacing a1: CoreGraphics.CGFloat?) { _skeleton() }
}

extension SwiftUI._VStackLayout: SwiftUI._VariadicView_UnaryViewRoot {
}

extension SwiftUI._VStackLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._VStackLayout: SwiftUI._VariadicView_ViewRoot {
}

extension SwiftUI._VStackLayout: SwiftUI._VariadicView_Root {
}

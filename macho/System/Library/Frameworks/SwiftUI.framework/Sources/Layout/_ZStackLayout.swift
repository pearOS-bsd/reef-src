// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ZStackLayout (Layout).

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

public struct _ZStackLayout {
    public var alignment: SwiftUI.Alignment
    public init(alignment a0: SwiftUI.Alignment) { _skeleton() }
}

extension SwiftUI._ZStackLayout: SwiftUI._VariadicView_UnaryViewRoot {
}

extension SwiftUI._ZStackLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._ZStackLayout: SwiftUI._VariadicView_ViewRoot {
}

extension SwiftUI._ZStackLayout: SwiftUI._VariadicView_Root {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _LayoutRoot (Layout).

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

public struct _LayoutRoot<A> where A: SwiftUI.Layout {
    public var layout: A
    func element(_ a0: [SwiftUI.Element]) -> SwiftUI.Element { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}

extension SwiftUI._LayoutRoot: SwiftUI.AnyLayoutRoot {
}

extension SwiftUI._LayoutRoot: SwiftUI._VariadicView_UnaryViewRoot {
}

extension SwiftUI._LayoutRoot: SwiftUI._VariadicView_ViewRoot {
}

extension SwiftUI._LayoutRoot: SwiftUI._VariadicView_Root {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _InsetViewModifier (Layout).

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

public struct _InsetViewModifier<A> where A: SwiftUI.View {
    public var content: A
    public var properties: (regions: SwiftUI.SafeAreaRegions, spacing: CoreGraphics.CGFloat?, edge: SwiftUI.Edge, alignment: SwiftUI.AlignmentKey)
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(content a0: A, regions a1: SwiftUI.SafeAreaRegions, spacing a2: CoreGraphics.CGFloat?, edge a3: SwiftUI.Edge, alignment a4: SwiftUI.AlignmentKey) { _skeleton() }
}

extension SwiftUI._InsetViewModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._InsetViewModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

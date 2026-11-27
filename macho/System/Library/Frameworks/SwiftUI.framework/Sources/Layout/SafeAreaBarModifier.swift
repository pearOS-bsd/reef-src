// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SafeAreaBarModifier (Layout).

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

struct SafeAreaBarModifier<A> where A: SwiftUI.View {
    let bar: A
    let edge: SwiftUI.Edge
    let spacing: CoreGraphics.CGFloat
    let alignment: SwiftUI.AlignmentKey
    init(bar a0: A, edge a1: SwiftUI.Edge, spacing a2: CoreGraphics.CGFloat, alignment a3: SwiftUI.AlignmentKey) { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.SafeAreaBarModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.SafeAreaBarModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}

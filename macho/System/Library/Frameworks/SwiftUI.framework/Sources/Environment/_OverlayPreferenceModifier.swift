// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OverlayPreferenceModifier (Environment).

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

public struct _OverlayPreferenceModifier<A, B> where A: SwiftUI.PreferenceKey, B: SwiftUI.View {
    public var alignment: SwiftUI.Alignment
    public var transform: (A.Value) -> B
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(alignment a0: SwiftUI.Alignment, transform a1: @escaping (A.Value) -> B) { _skeleton() }
}

extension SwiftUI._OverlayPreferenceModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._OverlayPreferenceModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

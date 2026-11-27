// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _AnchorTransformModifier (Layout).

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

public struct _AnchorTransformModifier<A, B> where B: SwiftUI.PreferenceKey {
    public let anchor: SwiftUI.Anchor<A>.Source
    public let transform: (inout B.Value, SwiftUI.Anchor<A>) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(anchor a0: SwiftUI.Anchor<A>.Source, transform a1: @escaping (inout B.Value, SwiftUI.Anchor<A>) -> ()) { _skeleton() }
}

extension SwiftUI._AnchorTransformModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._AnchorTransformModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

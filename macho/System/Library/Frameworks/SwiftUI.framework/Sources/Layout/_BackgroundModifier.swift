// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _BackgroundModifier (Layout).

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

public struct _BackgroundModifier<A> where A: SwiftUI.View {
    public var background: A
    public var alignment: SwiftUI.Alignment
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(background a0: A, alignment a1: SwiftUI.Alignment) { _skeleton() }
}

extension SwiftUI._BackgroundModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._BackgroundModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewThatFits (Views).

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

public struct ViewThatFits<A> where A: SwiftUI.View {
    let content: A
    public init(`in` a0: SwiftUI.Axis.Set, content a1: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ViewThatFits: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ViewThatFits: SwiftUI.PrimitiveView {
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Spacer (Layout).

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

public struct Spacer {
    public var minLength: CoreGraphics.CGFloat?
    public init(minLength a0: CoreGraphics.CGFloat?) { _skeleton() }
}

extension SwiftUI.Spacer: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Spacer: SwiftUI.View {
    public typealias Body = Swift.Never
}

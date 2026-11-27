// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _GridUnsizedAxesModifier (Layout).

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

public struct _GridUnsizedAxesModifier {
    public var axes: SwiftUI.Axis.Set
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(axes a0: SwiftUI.Axis.Set) { _skeleton() }
}

extension SwiftUI._GridUnsizedAxesModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._GridUnsizedAxesModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

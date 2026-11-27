// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _SafeAreaRegionsIgnoringLayout (Layout).

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

public struct _SafeAreaRegionsIgnoringLayout {
    public var regions: SwiftUI.SafeAreaRegions
    public var edges: SwiftUI.Edge.Set
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(regions a0: SwiftUI.SafeAreaRegions, edges a1: SwiftUI.Edge.Set) { _skeleton() }
}

extension SwiftUI._SafeAreaRegionsIgnoringLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._SafeAreaRegionsIgnoringLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._SafeAreaRegionsIgnoringLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

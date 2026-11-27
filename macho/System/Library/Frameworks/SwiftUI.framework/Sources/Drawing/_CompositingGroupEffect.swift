// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _CompositingGroupEffect (Drawing).

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

public struct _CompositingGroupEffect {
    public init() { _skeleton() }
}

extension SwiftUI._CompositingGroupEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._CompositingGroupEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._CompositingGroupEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

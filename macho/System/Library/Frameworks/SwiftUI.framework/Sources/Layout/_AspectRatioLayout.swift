// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _AspectRatioLayout (Layout).

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

public struct _AspectRatioLayout {
    public var aspectRatio: CoreGraphics.CGFloat?
    public var contentMode: SwiftUI.ContentMode
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(aspectRatio a0: CoreGraphics.CGFloat?, contentMode a1: SwiftUI.ContentMode) { _skeleton() }
}

extension SwiftUI._AspectRatioLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._AspectRatioLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._AspectRatioLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _FlexFrameLayout (Layout).

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

public struct _FlexFrameLayout {
    public let minWidth: CoreGraphics.CGFloat?
    public let idealWidth: CoreGraphics.CGFloat?
    public let maxWidth: CoreGraphics.CGFloat?
    public let minHeight: CoreGraphics.CGFloat?
    public let idealHeight: CoreGraphics.CGFloat?
    public let maxHeight: CoreGraphics.CGFloat?
    public let alignment: SwiftUI.Alignment
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(minWidth a0: CoreGraphics.CGFloat?, idealWidth a1: CoreGraphics.CGFloat?, maxWidth a2: CoreGraphics.CGFloat?, minHeight a3: CoreGraphics.CGFloat?, idealHeight a4: CoreGraphics.CGFloat?, maxHeight a5: CoreGraphics.CGFloat?, alignment a6: SwiftUI.Alignment) { _skeleton() }
}

extension SwiftUI._FlexFrameLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._FlexFrameLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._FlexFrameLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

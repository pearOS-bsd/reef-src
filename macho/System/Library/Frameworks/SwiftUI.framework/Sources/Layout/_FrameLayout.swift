// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _FrameLayout (Layout).

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

public struct _FrameLayout {
    public let width: CoreGraphics.CGFloat?
    public let height: CoreGraphics.CGFloat?
    public let alignment: SwiftUI.Alignment
    /// the content (under "frame") as one element in a frame of fixed width / height
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] {
        var context = a0
        context.path.append(AnyHashable("frame"))
        return [SwiftUI.FrameElement(SwiftUI.singleElement(a1(context)), width: width, height: height, alignment: alignment)]
    }
    public init(width a0: CoreGraphics.CGFloat?, height a1: CoreGraphics.CGFloat?, alignment a2: SwiftUI.Alignment) {
        width = a0
        height = a1
        alignment = a2
    }
}

extension SwiftUI._FrameLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._FrameLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._FrameLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

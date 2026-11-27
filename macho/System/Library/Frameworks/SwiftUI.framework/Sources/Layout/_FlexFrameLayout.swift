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
    /// the content (under "frame") as one element in a flexible frame
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] {
        var context = a0
        context.path.append(AnyHashable("frame"))
        return [SwiftUI.FlexFrameElement(SwiftUI.singleElement(a1(context)), layout: self)]
    }
    public init(minWidth a0: CoreGraphics.CGFloat?, idealWidth a1: CoreGraphics.CGFloat?, maxWidth a2: CoreGraphics.CGFloat?, minHeight a3: CoreGraphics.CGFloat?, idealHeight a4: CoreGraphics.CGFloat?, maxHeight a5: CoreGraphics.CGFloat?, alignment a6: SwiftUI.Alignment) {
        minWidth = a0
        idealWidth = a1
        maxWidth = a2
        minHeight = a3
        idealHeight = a4
        maxHeight = a5
        alignment = a6
    }
}

extension SwiftUI._FlexFrameLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._FlexFrameLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._FlexFrameLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

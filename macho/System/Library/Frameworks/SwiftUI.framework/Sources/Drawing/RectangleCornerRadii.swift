// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RectangleCornerRadii (Drawing).

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

public struct RectangleCornerRadii {
    public var topLeft: CoreGraphics.CGFloat
    public var topRight: CoreGraphics.CGFloat
    public var bottomRight: CoreGraphics.CGFloat
    public var bottomLeft: CoreGraphics.CGFloat
    public init(topLeading a0: CoreGraphics.CGFloat, bottomLeading a1: CoreGraphics.CGFloat, bottomTrailing a2: CoreGraphics.CGFloat, topTrailing a3: CoreGraphics.CGFloat) { _skeleton() }
    public var topLeading: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var bottomLeading: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var bottomTrailing: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var topTrailing: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public static func == (_ a0: SwiftUI.RectangleCornerRadii, _ a1: SwiftUI.RectangleCornerRadii) -> Swift.Bool { _skeleton() }
    public init(topLeft a0: CoreGraphics.CGFloat, topRight a1: CoreGraphics.CGFloat, bottomRight a2: CoreGraphics.CGFloat, bottomLeft a3: CoreGraphics.CGFloat) { _skeleton() }
}

extension SwiftUI.RectangleCornerRadii: Swift.Equatable {
}

extension SwiftUI.RectangleCornerRadii: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

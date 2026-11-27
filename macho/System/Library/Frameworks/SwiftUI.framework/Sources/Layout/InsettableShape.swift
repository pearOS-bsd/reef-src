// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// InsettableShape (Layout).

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

public protocol InsettableShape: SwiftUI.Shape {
    associatedtype InsetShape: SwiftUI.InsettableShape
    func inset(by a0: CoreGraphics.CGFloat) -> Self.InsetShape
}

extension SwiftUI.InsettableShape {
    public func strokeBorder<A1>(_ a0: A1, style a1: SwiftUI.StrokeStyle, antialiased a2: Swift.Bool) -> SwiftUI.StrokeBorderShapeView<Self, A1, SwiftUI.EmptyView> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func strokeBorder<A1>(_ a0: A1, lineWidth a1: CoreGraphics.CGFloat, antialiased a2: Swift.Bool) -> SwiftUI.StrokeBorderShapeView<Self, A1, SwiftUI.EmptyView> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func strokeBorder(style a0: SwiftUI.StrokeStyle, antialiased a1: Swift.Bool) -> some SwiftUI.View { _skeleton() }
    public func strokeBorder(lineWidth a0: CoreGraphics.CGFloat, antialiased a1: Swift.Bool) -> some SwiftUI.View { _skeleton() }
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TrimmedShape (Drawing).

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

public struct _TrimmedShape<A> where A: SwiftUI.Shape {
    public var shape: A
    public var startFraction: CoreGraphics.CGFloat
    public var endFraction: CoreGraphics.CGFloat
    public init(shape a0: A, startFraction a1: CoreGraphics.CGFloat, endFraction a2: CoreGraphics.CGFloat) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI._TrimmedShape: SwiftUI.Shape {
}

extension SwiftUI._TrimmedShape: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._TrimmedShape: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI._TrimmedShape<A>, SwiftUI.ForegroundStyle>
}

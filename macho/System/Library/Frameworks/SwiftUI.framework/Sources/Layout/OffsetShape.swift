// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OffsetShape (Layout).

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

public struct OffsetShape<A> where A: SwiftUI.Shape {
    public var shape: A
    public var offset: CGSize
    public init(shape a0: A, offset a1: CGSize) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI.OffsetShape: SwiftUI.Shape {
}

extension SwiftUI.OffsetShape: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.OffsetShape: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.OffsetShape<A>, SwiftUI.ForegroundStyle>
}

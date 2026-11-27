// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyShape (Drawing).

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

public struct AnyShape {
    let pathIn: (CGRect) -> SwiftUI.Path
    public init<A>(_ a0: A) where A: SwiftUI.Shape { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI.AnyShape: SwiftUI.Shape {
}

extension SwiftUI.AnyShape: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.AnyShape: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.AnyShape, SwiftUI.ForegroundStyle>
}

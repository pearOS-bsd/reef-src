// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContainerRelativeShape (Layout).

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

public struct ContainerRelativeShape {
    public init() { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI.ContainerRelativeShape: SwiftUI.Shape {
}

extension SwiftUI.ContainerRelativeShape: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.ContainerRelativeShape: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.ContainerRelativeShape, SwiftUI.ForegroundStyle>
}

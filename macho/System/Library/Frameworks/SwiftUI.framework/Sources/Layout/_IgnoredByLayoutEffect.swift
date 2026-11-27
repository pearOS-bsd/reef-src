// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _IgnoredByLayoutEffect (Layout).

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

public struct _IgnoredByLayoutEffect<A> where A: SwiftUI.GeometryEffect {
    public var base: A
    public func effectValue(size a0: CGSize) -> SwiftUI.ProjectionTransform { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}

extension SwiftUI._IgnoredByLayoutEffect: SwiftUI.GeometryEffect {
}

extension SwiftUI._IgnoredByLayoutEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._IgnoredByLayoutEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OffsetEffect (Layout).

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

public struct _OffsetEffect {
    public var offset: CGSize
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(offset a0: CGSize) { _skeleton() }
    public func effectValue(size a0: CGSize) -> SwiftUI.ProjectionTransform { _skeleton() }
    public static func == (_ a0: SwiftUI._OffsetEffect, _ a1: SwiftUI._OffsetEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._OffsetEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._OffsetEffect: SwiftUI.GeometryEffect {
}

extension SwiftUI._OffsetEffect: Swift.Equatable {
}

extension SwiftUI._OffsetEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._OffsetEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

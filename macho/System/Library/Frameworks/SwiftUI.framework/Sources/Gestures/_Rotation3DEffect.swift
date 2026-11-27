// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _Rotation3DEffect (Gestures).

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

public struct _Rotation3DEffect {
    public var angle: SwiftUI.Angle
    public var axis: (x: CoreGraphics.CGFloat, y: CoreGraphics.CGFloat, z: CoreGraphics.CGFloat)
    public var anchor: SwiftUI.UnitPoint
    public var anchorZ: CoreGraphics.CGFloat
    public var perspective: CoreGraphics.CGFloat
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(angle a0: SwiftUI.Angle, axis a1: (x: CoreGraphics.CGFloat, y: CoreGraphics.CGFloat, z: CoreGraphics.CGFloat), anchor a2: SwiftUI.UnitPoint, anchorZ a3: CoreGraphics.CGFloat, perspective a4: CoreGraphics.CGFloat) { _skeleton() }
    public func effectValue(size a0: CGSize) -> SwiftUI.ProjectionTransform { _skeleton() }
    public static func == (_ a0: SwiftUI._Rotation3DEffect, _ a1: SwiftUI._Rotation3DEffect) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._Rotation3DEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._Rotation3DEffect: SwiftUI.GeometryEffect {
}

extension SwiftUI._Rotation3DEffect: Swift.Equatable {
}

extension SwiftUI._Rotation3DEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._Rotation3DEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

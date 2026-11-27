// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _RotationEffect (Gestures).

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

public struct _RotationEffect {
    public var angle: SwiftUI.Angle
    public var anchor: SwiftUI.UnitPoint
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public func effectValue(size a0: CGSize) -> SwiftUI.ProjectionTransform { _skeleton() }
    public var animatableData: SwiftUI.AnimatablePair<Swift.Double, SwiftUI.AnimatablePair<CoreGraphics.CGFloat, CoreGraphics.CGFloat>> {
        get { _skeleton() }
        set { _skeleton() }
    }
    public static func == (_ a0: SwiftUI._RotationEffect, _ a1: SwiftUI._RotationEffect) -> Swift.Bool { _skeleton() }
    public init(angle a0: SwiftUI.Angle, anchor a1: SwiftUI.UnitPoint) { _skeleton() }
}

extension SwiftUI._RotationEffect: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._RotationEffect: SwiftUI.GeometryEffect {
}

extension SwiftUI._RotationEffect: Swift.Equatable {
}

extension SwiftUI._RotationEffect: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.AnimatablePair<Swift.Double, SwiftUI.AnimatablePair<CoreGraphics.CGFloat, CoreGraphics.CGFloat>>
}

extension SwiftUI._RotationEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MeshGradient (Drawing).

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

public struct MeshGradient {
    public var width: Swift.Int
    public var height: Swift.Int
    public var locations: SwiftUI.MeshGradient.Locations
    public var colors: SwiftUI.MeshGradient.Colors
    public var background: SwiftUI.Color
    public var smoothsColors: Swift.Bool
    public var colorSpace: SwiftUI.Gradient.ColorSpace
    public init(width a0: Swift.Int, height a1: Swift.Int, locations a2: SwiftUI.MeshGradient.Locations, colors a3: SwiftUI.MeshGradient.Colors, background a4: SwiftUI.Color, smoothsColors a5: Swift.Bool, colorSpace a6: SwiftUI.Gradient.ColorSpace) { _skeleton() }
    public init(width a0: Swift.Int, height a1: Swift.Int, points a2: [Swift.SIMD2<Swift.Float>], colors a3: [SwiftUI.Color], background a4: SwiftUI.Color, smoothsColors a5: Swift.Bool, colorSpace a6: SwiftUI.Gradient.ColorSpace) { _skeleton() }
    public init(width a0: Swift.Int, height a1: Swift.Int, points a2: [Swift.SIMD2<Swift.Float>], resolvedColors a3: [SwiftUI.Color.Resolved], background a4: SwiftUI.Color, smoothsColors a5: Swift.Bool, colorSpace a6: SwiftUI.Gradient.ColorSpace) { _skeleton() }
    public init(width a0: Swift.Int, height a1: Swift.Int, bezierPoints a2: [SwiftUI.MeshGradient.BezierPoint], colors a3: [SwiftUI.Color], background a4: SwiftUI.Color, smoothsColors a5: Swift.Bool, colorSpace a6: SwiftUI.Gradient.ColorSpace) { _skeleton() }
    public init(width a0: Swift.Int, height a1: Swift.Int, bezierPoints a2: [SwiftUI.MeshGradient.BezierPoint], resolvedColors a3: [SwiftUI.Color.Resolved], background a4: SwiftUI.Color, smoothsColors a5: Swift.Bool, colorSpace a6: SwiftUI.Gradient.ColorSpace) { _skeleton() }
    public var body: SwiftUI.Canvas<SwiftUI.EmptyView> { _skeleton() }
    func resolvedColors(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.Color.Resolved] { _skeleton() }
    public static func == (_ a0: SwiftUI.MeshGradient, _ a1: SwiftUI.MeshGradient) -> Swift.Bool { _skeleton() }
    public struct BezierPoint {
        public var position: Swift.SIMD2<Swift.Float>
        public var leadingControlPoint: Swift.SIMD2<Swift.Float>
        public var topControlPoint: Swift.SIMD2<Swift.Float>
        public var trailingControlPoint: Swift.SIMD2<Swift.Float>
        public var bottomControlPoint: Swift.SIMD2<Swift.Float>
        public init(position a0: Swift.SIMD2<Swift.Float>, leadingControlPoint a1: Swift.SIMD2<Swift.Float>, topControlPoint a2: Swift.SIMD2<Swift.Float>, trailingControlPoint a3: Swift.SIMD2<Swift.Float>, bottomControlPoint a4: Swift.SIMD2<Swift.Float>) { _skeleton() }
        public static func == (_ a0: SwiftUI.MeshGradient.BezierPoint, _ a1: SwiftUI.MeshGradient.BezierPoint) -> Swift.Bool { _skeleton() }
    }
    public enum Locations {
        case points([Swift.SIMD2<Swift.Float>])
        case bezierPoints([SwiftUI.MeshGradient.BezierPoint])
        public static func == (_ a0: SwiftUI.MeshGradient.Locations, _ a1: SwiftUI.MeshGradient.Locations) -> Swift.Bool { _skeleton() }
    }
    public enum Colors {
        case colors([SwiftUI.Color])
        case resolvedColors([SwiftUI.Color.Resolved])
        public static func == (_ a0: SwiftUI.MeshGradient.Colors, _ a1: SwiftUI.MeshGradient.Colors) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.MeshGradient.BezierPoint: Swift.Equatable {
}

extension SwiftUI.MeshGradient.Locations: Swift.Equatable {
}

extension SwiftUI.MeshGradient.Colors: Swift.Equatable {
}

extension SwiftUI.MeshGradient: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}

extension SwiftUI.MeshGradient: Swift.Equatable {
}

extension SwiftUI.MeshGradient: SwiftUI.View {
    public typealias Body = SwiftUI.Canvas<SwiftUI.EmptyView>
}

extension SwiftUI.MeshGradient: SwiftUI.ResolvableShapeStyle {
    func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
}

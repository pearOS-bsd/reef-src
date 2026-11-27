// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Path (Drawing).

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

public struct Path {
    public var storage: SwiftUI.Path.Storage
    public var boundingRect: CGRect { _skeleton() }
    public func applying(_ a0: CGAffineTransform) -> SwiftUI.Path { _skeleton() }
    public var cgPath: CGPath { _skeleton() }
    public func strokedPath(_ a0: SwiftUI.StrokeStyle) -> SwiftUI.Path { _skeleton() }
    public init() { _skeleton() }
    public init(roundedRect a0: CGRect, cornerRadius a1: CoreGraphics.CGFloat, style a2: SwiftUI.RoundedCornerStyle) { _skeleton() }
    public init(_ a0: (inout SwiftUI.Path) -> ()) { _skeleton() }
    public init(_ a0: CGPath) { _skeleton() }
    var elements: [SwiftUI.Path.Element] {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var isEmpty: Swift.Bool { _skeleton() }
    public static func == (_ a0: SwiftUI.Path, _ a1: SwiftUI.Path) -> Swift.Bool { _skeleton() }
    public func addArc(center a0: CGPoint, radius a1: CoreGraphics.CGFloat, startAngle a2: SwiftUI.Angle, endAngle a3: SwiftUI.Angle, clockwise a4: Swift.Bool, transform a5: CGAffineTransform) { _skeleton() }
    public init(ellipseIn a0: CGRect) { _skeleton() }
    public func move(to a0: CGPoint) { _skeleton() }
    public func addLine(to a0: CGPoint) { _skeleton() }
    public func closeSubpath() { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public func addRoundedRect(`in` a0: CGRect, cornerRadii a1: SwiftUI.RectangleCornerRadii, style a2: SwiftUI.RoundedCornerStyle, transform a3: CGAffineTransform) { _skeleton() }
    public func addRelativeArc(center a0: CGPoint, radius a1: CoreGraphics.CGFloat, startAngle a2: SwiftUI.Angle, delta a3: SwiftUI.Angle, transform a4: CGAffineTransform) { _skeleton() }
    public func addCurve(to a0: CGPoint, control1 a1: CGPoint, control2 a2: CGPoint) { _skeleton() }
    public func addRect(_ a0: CGRect, transform a1: CGAffineTransform) { _skeleton() }
    public func addEllipse(`in` a0: CGRect, transform a1: CGAffineTransform) { _skeleton() }
    public func addRoundedRect(`in` a0: CGRect, cornerSize a1: CGSize, style a2: SwiftUI.RoundedCornerStyle, transform a3: CGAffineTransform) { _skeleton() }
    public func addPath(_ a0: SwiftUI.Path, transform a1: CGAffineTransform) { _skeleton() }
    public func addQuadCurve(to a0: CGPoint, control a1: CGPoint) { _skeleton() }
    public func addLines(_ a0: [CGPoint]) { _skeleton() }
    public init(_ a0: CGRect) { _skeleton() }
    @frozen public enum Storage {
        case rect(CGRect)
        case ellipse(CGRect)
        case path(SwiftUI.PathBox)
        case empty
        public static func == (_ a0: SwiftUI.Path.Storage, _ a1: SwiftUI.Path.Storage) -> Swift.Bool { _skeleton() }
    }
    enum Element {
        case move(CGPoint)
        case line(CGPoint)
        case quadCurve(CGPoint, CGPoint)
        case curve(CGPoint, CGPoint, CGPoint)
        case rect(CGRect)
        case ellipse(CGRect)
        case roundedRect(CGRect, cornerRadius: CoreGraphics.CGFloat, style: SwiftUI.RoundedCornerStyle)
        case cgPath(CGPath)
        case close
        static func == (_ a0: SwiftUI.Path.Element, _ a1: SwiftUI.Path.Element) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.Path.Storage: Swift.Equatable {
}

extension SwiftUI.Path: Swift.Equatable {
}

extension SwiftUI.Path: SwiftUI.Shape {
}

extension SwiftUI.Path: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Path: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Path, SwiftUI.ForegroundStyle>
}

extension SwiftUI.Path.Element: Swift.Equatable {
}

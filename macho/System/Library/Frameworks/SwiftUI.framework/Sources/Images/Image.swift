// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Image (Images).

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

public struct Image {
    public var provider: SwiftUI.AnyImageProviderBox
    public func resizable(capInsets a0: SwiftUI.EdgeInsets, resizingMode a1: SwiftUI.Image.ResizingMode) -> SwiftUI.Image { _skeleton() }
    public static var transferRepresentation: CoreTransferable.ProxyRepresentation<SwiftUI.Image, Foundation.Data> { _skeleton() }
    public init(systemName a0: Swift.String) { _skeleton() }
    public init(nsImage a0: NSImage) { _skeleton() }
    public init(size a0: CGSize, label a1: SwiftUI.Text?, opaque a2: Swift.Bool, colorMode a3: SwiftUI.ColorRenderingMode, renderer a4: @escaping (inout SwiftUI.GraphicsContext) -> ()) { _skeleton() }
    public func renderingMode(_ a0: SwiftUI.Image.TemplateRenderingMode?) -> SwiftUI.Image { _skeleton() }
    public init(decorative a0: Swift.String, bundle a1: Bundle?) { _skeleton() }
    public init(decorative a0: CGImage, scale a1: CoreGraphics.CGFloat, orientation a2: SwiftUI.Image.Orientation) { _skeleton() }
    public init(systemName a0: Swift.String, variableValue a1: Swift.Double?) { _skeleton() }
    public init(_ a0: DeveloperToolsSupport.ImageResource) { _skeleton() }
    public func symbolRenderingMode(_ a0: SwiftUI.SymbolRenderingMode?) -> SwiftUI.Image { _skeleton() }
    public func interpolation(_ a0: SwiftUI.Image.Interpolation) -> SwiftUI.Image { _skeleton() }
    public func antialiased(_ a0: Swift.Bool) -> SwiftUI.Image { _skeleton() }
    public static func == (_ a0: SwiftUI.Image, _ a1: SwiftUI.Image) -> Swift.Bool { _skeleton() }
    public init(_ a0: Swift.String, bundle a1: Bundle?) { _skeleton() }
    public enum ResizingMode {
        case tile
        case stretch
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Image.ResizingMode, _ a1: SwiftUI.Image.ResizingMode) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    public enum Scale {
        case small
        case medium
        case large
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Image.Scale, _ a1: SwiftUI.Image.Scale) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    public enum TemplateRenderingMode {
        case template
        case original
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Image.TemplateRenderingMode, _ a1: SwiftUI.Image.TemplateRenderingMode) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    public enum Orientation {
        case up
        case upMirrored
        case down
        case downMirrored
        case left
        case leftMirrored
        case right
        case rightMirrored
        public init?(rawValue a0: Swift.UInt8) { _skeleton() }
        public static var allCases: [SwiftUI.Image.Orientation] { _skeleton() }
        public var rawValue: Swift.UInt8 { _skeleton() }
    }
    public enum Interpolation {
        case none
        case low
        case medium
        case high
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Image.Interpolation, _ a1: SwiftUI.Image.Interpolation) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
}

extension SwiftUI.Image: CoreTransferable.Transferable {
    public typealias Representation = CoreTransferable.ProxyRepresentation<SwiftUI.Image, Foundation.Data>
}

extension SwiftUI.Image: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Image: Swift.Equatable {
}

extension SwiftUI.Image: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Image.ResizingMode: Swift.Equatable {
}

extension SwiftUI.Image.ResizingMode: Swift.Hashable {
}

extension SwiftUI.Image.Scale: Swift.Equatable {
}

extension SwiftUI.Image.Scale: Swift.Hashable {
}

extension SwiftUI.Image.TemplateRenderingMode: Swift.Equatable {
}

extension SwiftUI.Image.TemplateRenderingMode: Swift.Hashable {
}

extension SwiftUI.Image.Orientation: Swift.Equatable {
}

extension SwiftUI.Image.Orientation: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt8
}

extension SwiftUI.Image.Orientation: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.Image.Orientation]
}

extension SwiftUI.Image.Orientation: Swift.Hashable {
}

extension SwiftUI.Image.Interpolation: Swift.Equatable {
}

extension SwiftUI.Image.Interpolation: Swift.Hashable {
}

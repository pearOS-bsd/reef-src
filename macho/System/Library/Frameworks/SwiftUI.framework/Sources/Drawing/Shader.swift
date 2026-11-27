// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Shader (Drawing).

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

public struct Shader {
    public var function: SwiftUI.ShaderFunction
    public var arguments: [SwiftUI.Shader.Argument]
    public var dithersColor: Swift.Bool
    public init(function a0: SwiftUI.ShaderFunction, arguments a1: [SwiftUI.Shader.Argument]) { _skeleton() }
    public func compile(`as` a0: SwiftUI.Shader.UsageType) async throws { _skeleton() }
    public static func == (_ a0: SwiftUI.Shader, _ a1: SwiftUI.Shader) -> Swift.Bool { _skeleton() }
    public struct Argument {
        let value: SwiftUI.Shader.Argument.Value
        public static func == (_ a0: SwiftUI.Shader.Argument, _ a1: SwiftUI.Shader.Argument) -> Swift.Bool { _skeleton() }
        public static func _float(_ a0: Swift.Float) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func _float2(_ a0: Swift.Float, _ a1: Swift.Float) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func _float3(_ a0: Swift.Float, _ a1: Swift.Float, _ a2: Swift.Float) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func _float4(_ a0: Swift.Float, _ a1: Swift.Float, _ a2: Swift.Float, _ a3: Swift.Float) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func float<A>(_ a0: A) -> SwiftUI.Shader.Argument where A: Swift.BinaryFloatingPoint { _skeleton() }
        public static func float2<A>(_ a0: A, _ a1: A) -> SwiftUI.Shader.Argument where A: Swift.BinaryFloatingPoint { _skeleton() }
        public static func float3<A>(_ a0: A, _ a1: A, _ a2: A) -> SwiftUI.Shader.Argument where A: Swift.BinaryFloatingPoint { _skeleton() }
        public static func float4<A>(_ a0: A, _ a1: A, _ a2: A, _ a3: A) -> SwiftUI.Shader.Argument where A: Swift.BinaryFloatingPoint { _skeleton() }
        public static func float2(_ a0: CGSize) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func floatArray(_ a0: [Swift.Float]) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func color(_ a0: SwiftUI.Color) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func colorArray(_ a0: [SwiftUI.Color]) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func image(_ a0: SwiftUI.Image) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func data(_ a0: Foundation.Data) -> SwiftUI.Shader.Argument { _skeleton() }
        public static var boundingRect: SwiftUI.Shader.Argument { _skeleton() }
        public static func float2(_ a0: CGPoint) -> SwiftUI.Shader.Argument { _skeleton() }
        public static func float2(_ a0: CGVector) -> SwiftUI.Shader.Argument { _skeleton() }
        enum Value {
            case floats([Swift.Float])
            case color(SwiftUI.Color)
            case floatArray([Swift.Float])
            case colorArray([SwiftUI.Color])
            case image(SwiftUI.Image)
            case data(Foundation.Data)
            case boundingRect
        }
    }
    public struct UsageType {
        let effect: Swift.Int
        public static var colorEffect: SwiftUI.Shader.UsageType { _skeleton() }
        public static var distortionEffect: SwiftUI.Shader.UsageType { _skeleton() }
        public static var layerEffect: SwiftUI.Shader.UsageType { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public static func == (_ a0: SwiftUI.Shader.UsageType, _ a1: SwiftUI.Shader.UsageType) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    }
}

extension SwiftUI.Shader.Argument: Swift.Equatable {
}

extension SwiftUI.Shader.UsageType: Swift.Hashable {
}

extension SwiftUI.Shader.UsageType: Swift.Equatable {
}

extension SwiftUI.Shader: Swift.Equatable {
}

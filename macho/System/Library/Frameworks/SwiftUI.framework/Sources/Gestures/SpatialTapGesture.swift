// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpatialTapGesture (Gestures).

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

public struct SpatialTapGesture {
    public var count: Swift.Int
    public var coordinateSpace: SwiftUI.CoordinateSpace
    public init(count a0: Swift.Int, coordinateSpace a1: SwiftUI.CoordinateSpace) { _skeleton() }
    public init<A>(count a0: Swift.Int, coordinateSpace a1: A) where A: SwiftUI.CoordinateSpaceProtocol { _skeleton() }
    public struct Value {
        public var location: CGPoint
        public static func == (_ a0: SwiftUI.SpatialTapGesture.Value, _ a1: SwiftUI.SpatialTapGesture.Value) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.SpatialTapGesture: SwiftUI.RecognizableGesture {
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
}

extension SwiftUI.SpatialTapGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}

extension SwiftUI.SpatialTapGesture.Value: Swift.Equatable {
}

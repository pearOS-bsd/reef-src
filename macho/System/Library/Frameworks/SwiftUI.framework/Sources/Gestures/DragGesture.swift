// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DragGesture (Gestures).

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

public struct DragGesture {
    public var minimumDistance: CoreGraphics.CGFloat
    public var coordinateSpace: SwiftUI.CoordinateSpace
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    public init(minimumDistance a0: CoreGraphics.CGFloat, coordinateSpace a1: SwiftUI.CoordinateSpace) { _skeleton() }
    public init<A>(minimumDistance a0: CoreGraphics.CGFloat, coordinateSpace a1: A) where A: SwiftUI.CoordinateSpaceProtocol { _skeleton() }
    public struct Value {
        public var time: Foundation.Date
        public var location: CGPoint
        public var startLocation: CGPoint
        public var translation: CGSize { _skeleton() }
        public static func == (_ a0: SwiftUI.DragGesture.Value, _ a1: SwiftUI.DragGesture.Value) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.DragGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI.DragGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}

extension SwiftUI.DragGesture.Value: Swift.Equatable {
}

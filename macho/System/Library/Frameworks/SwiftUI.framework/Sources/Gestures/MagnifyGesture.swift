// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MagnifyGesture (Gestures).

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

public struct MagnifyGesture {
    public var minimumScaleDelta: CoreGraphics.CGFloat
    public init(minimumScaleDelta a0: CoreGraphics.CGFloat) { _skeleton() }
    public struct Value {
        public var time: Foundation.Date
        public var magnification: CoreGraphics.CGFloat
        public var velocity: CoreGraphics.CGFloat
        public var startAnchor: SwiftUI.UnitPoint
        public var startLocation: CGPoint
        public static func == (_ a0: SwiftUI.MagnifyGesture.Value, _ a1: SwiftUI.MagnifyGesture.Value) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.MagnifyGesture.Value: Swift.Equatable {
}

extension SwiftUI.MagnifyGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}

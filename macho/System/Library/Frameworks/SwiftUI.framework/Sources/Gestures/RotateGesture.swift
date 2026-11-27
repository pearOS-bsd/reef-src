// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RotateGesture (Gestures).

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

public struct RotateGesture {
    public var minimumAngleDelta: SwiftUI.Angle
    public init(minimumAngleDelta a0: SwiftUI.Angle) { _skeleton() }
    public struct Value {
        public var time: Foundation.Date
        public var rotation: SwiftUI.Angle
        public var velocity: SwiftUI.Angle
        public var startAnchor: SwiftUI.UnitPoint
        public var startLocation: CGPoint
        public static func == (_ a0: SwiftUI.RotateGesture.Value, _ a1: SwiftUI.RotateGesture.Value) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.RotateGesture.Value: Swift.Equatable {
}

extension SwiftUI.RotateGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}

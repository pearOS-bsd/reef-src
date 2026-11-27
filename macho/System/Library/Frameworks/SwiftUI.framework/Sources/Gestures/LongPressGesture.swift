// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LongPressGesture (Gestures).

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

public struct LongPressGesture {
    public var minimumDuration: Swift.Double
    public var maximumDistance: CoreGraphics.CGFloat
    public init(minimumDuration a0: Swift.Double, maximumDistance a1: CoreGraphics.CGFloat) { _skeleton() }
}

extension SwiftUI.LongPressGesture: SwiftUI.RecognizableGesture {
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
}

extension SwiftUI.LongPressGesture: SwiftUI.Gesture {
    public typealias Value = Swift.Bool
    public typealias Body = Swift.Never
}

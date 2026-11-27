// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SimultaneousGesture (Gestures).

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

public struct SimultaneousGesture<A, B> where A: SwiftUI.Gesture, B: SwiftUI.Gesture {
    public var first: A
    public var second: B
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    public init(_ a0: A, _ a1: B) { _skeleton() }
    public struct Value {
        public var first: A.Value?
        public var second: B.Value?
    }
}

extension SwiftUI.SimultaneousGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI.SimultaneousGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}
